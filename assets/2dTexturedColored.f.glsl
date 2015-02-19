#version 120

varying vec2 v_texcoord;
varying vec4 v_color;
uniform sampler2D mytexture;
void main(void) {
	gl_FragColor = texture2D(mytexture, v_texcoord) * v_color;
}