#version 120

varying vec2 v_texcoord;
uniform sampler2D mytexture;
void main(void) {
	gl_FragColor = texture2D(mytexture, v_texcoord);
}