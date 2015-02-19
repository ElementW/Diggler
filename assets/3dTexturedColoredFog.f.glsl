#version 120

varying vec2 v_texcoord;
varying vec4 v_color;
varying vec4 v_coord;
uniform sampler2D texture;
uniform float fogEnd = 32.0;
uniform float fogStart = 16.0;

void main(void) {
	float fogCoord = (gl_FragCoord.z/gl_FragCoord.w);
	gl_FragColor = mix(texture2D(texture, v_texcoord) * v_color, vec4(0.0, 0.0, 0.0, 1.0), 1.0-clamp((fogEnd-fogCoord)/(fogEnd-fogStart), 0.0, 1.0));
}