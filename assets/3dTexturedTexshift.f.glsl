#version 120

varying vec2 v_texcoord;
uniform vec2 texshift;
uniform sampler2D texture;

void main(void) {
	gl_FragColor = texture2D(texture, v_texcoord + texshift);
}