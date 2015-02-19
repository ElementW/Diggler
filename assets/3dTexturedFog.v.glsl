#version 120

uniform mat4 mvp;
attribute vec4 coord;
attribute vec2 texcoord;
varying vec4 v_coord;
varying vec2 v_texcoord;

void main(void) {
	v_coord = coord;
	v_texcoord = texcoord;
	gl_Position = mvp * vec4(coord.xyz, 1);
}