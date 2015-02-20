attribute vec2 coord;
uniform mat4 mvp;
attribute vec2 texcoord;
varying vec2 v_texcoord;

void main(void) {
	gl_Position = mvp * vec4(coord, 0.0, 1.0);
	v_texcoord = texcoord;
}