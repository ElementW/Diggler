uniform vec4 unicolor = vec4(1.0, 1.0, 1.0, 1.0);
#ifdef COLORED
varying vec4 v_color;
#endif
#ifdef TEXTURED
varying vec2 v_texcoord;
uniform sampler2D mytexture;
#endif

void main(void) {
	gl_FragColor = unicolor;
#ifdef TEXTURED
	gl_FragColor *= texture2D(mytexture, v_texcoord);
#endif
#ifdef COLORED
	gl_FragColor *= v_color;
#endif
}