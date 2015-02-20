uniform mat4 mvp;
attribute vec4 coord;

#ifdef COLORED
attribute vec4 color;
varying vec4 v_color;
#endif
#ifdef TEXTURED
attribute vec2 texcoord;
varying vec2 v_texcoord;
#endif

void main(void) {
#ifdef COLORED
	v_color = color;
#endif
#ifdef TEXTURED
	v_texcoord = texcoord;
#endif
	gl_Position = mvp * vec4(coord.xyz, 1);
}