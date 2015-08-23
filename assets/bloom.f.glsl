#define t(a,b) texture2D(mytexture, clamp(v_texcoord+vec2(a*pixshift.x, b*pixshift.y), 0.0, 1.0))
precision lowp float;

varying vec2 v_texcoord;
uniform sampler2D mytexture;
uniform vec2 pixshift;

void main(void) {
	gl_FragColor = (
		t(-2.0, 2.0) + t(-1.0, 2.0) + t(0.0, 2.0) + t(1.0, 2.0) + t(2.0, 2.0) +
		t(-2.0, 1.0) + t(-1.0, 1.0) + t(0.0, 1.0) + t(1.0, 1.0) + t(2.0, 1.0) +
		t(-2.0, 0.0) + t(-1.0, 0.0) + t(0.0, 0.0) + t(1.0, 0.0) + t(2.0, 0.0) +
		t(-2.0,-1.0) + t(-1.0,-1.0) + t(0.0,-1.0) + t(1.0,-1.0) + t(2.0,-1.0) +
		t(-2.0,-2.0) + t(-1.0,-2.0) + t(0.0,-2.0) + t(1.0,-2.0) + t(2.0,-2.0)
	)/10.0;
}