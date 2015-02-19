#version 120

uniform mat4 mvp;
attribute vec4 coord;
attribute vec2 texcoord;
varying vec2 v_texcoord;

void main(void) {
	v_texcoord = texcoord;
	gl_Position = mvp * vec4(coord.xyz, 1);
}