attribute vec2 coord;
uniform mat4 mvp;

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
	gl_Position = mvp * vec4(coord, 0.0, 1.0);
}