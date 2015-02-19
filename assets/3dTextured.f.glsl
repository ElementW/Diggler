#version 120

varying vec2 v_texcoord;
uniform sampler2D texture;

void main(void) {
	gl_FragColor = texture2D(texture, v_texcoord);
}