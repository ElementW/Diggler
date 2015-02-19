#version 120

attribute vec2 coord;
uniform mat4 mvp;
attribute vec4 color;
varying vec4 v_color;

void main(void) {
	gl_Position = mvp * vec4(coord, 0.0, 1.0);
	v_color = color;
}