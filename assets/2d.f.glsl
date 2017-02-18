precision lowp float;

uniform vec4 unicolor;
#ifdef COLORED
varying vec4 v_color;
#endif
#ifdef TEXTURED
varying vec2 v_texcoord;
uniform sampler2D mytexture;
#endif
#ifdef ABERRATION
uniform vec4 aberration;
#endif

void main(void) {
	gl_FragColor = unicolor;
#ifdef TEXTURED
 #ifdef ABERRATION
	vec4 c = texture2D(mytexture, v_texcoord);
	gl_FragColor *= vec4(texture2D(mytexture, v_texcoord + aberration.xy).r, c.g, texture2D(mytexture, v_texcoord + aberration.zw).b, c.a);
 #else
	gl_FragColor *= texture2D(mytexture, v_texcoord);
 #endif
#endif
#ifdef COLORED
	gl_FragColor *= v_color;
#endif
}
