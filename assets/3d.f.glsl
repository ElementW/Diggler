uniform vec4 unicolor = vec4(1.0, 1.0, 1.0, 1.0);
#ifdef COLORED
varying vec4 v_color;
#endif
#ifdef TEXTURED
uniform sampler2D texture;
varying vec2 v_texcoord;
 #ifdef TEXSHIFT
uniform vec2 texshift;
 #endif
#endif
#ifdef FOG
uniform float fogEnd = 32.0;
uniform float fogStart = 16.0;
#endif

void main(void) {
	gl_FragColor = unicolor;
#ifdef TEXTURED
 #ifdef TEXSHIFT
	gl_FragColor *= texture2D(texture, v_texcoord + texshift);
 #else
	gl_FragColor *= texture2D(texture, v_texcoord);
 #endif
 #ifdef DISCARD
	if (gl_FragColor.a == 0.0)
		discard;
 #endif
#endif
#ifdef COLORED
	gl_FragColor *= v_color;
#endif
#ifdef FOG
	float fogCoord = (gl_FragCoord.z/gl_FragCoord.w);
	gl_FragColor = mix(gl_FragColor, vec4(0.0, 0.0, 0.0, 1.0), 1.0-clamp((fogEnd-fogCoord)/(fogEnd-fogStart), 0.0, 1.0));
#endif
}