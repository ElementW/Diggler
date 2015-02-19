#version 120

uniform mat4 mvp;
attribute vec4 coord;
attribute vec4 color;
attribute vec2 texcoord;
varying vec2 v_texcoord;
varying vec4 v_color;

void main(void) {
	v_texcoord = texcoord;
	v_color = color;
	gl_Position = mvp * vec4(coord.xyz, 1);
}