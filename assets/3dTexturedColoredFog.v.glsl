#version 120

uniform mat4 mvp;
attribute vec4 coord;
attribute vec2 texcoord;
attribute vec4 color;
varying vec2 v_texcoord;
varying vec4 v_color;

void main(void) {
	v_texcoord = texcoord;
	v_color = color;
	gl_Position = mvp * vec4(coord.xyz, 1);
}