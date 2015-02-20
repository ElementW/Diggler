#define t(a,b) texture2D(mytexture, clamp(v_texcoord+vec2(a*pixshift.x*1.1,b*pixshift.y*1.1), vec2(0.0,0.0), vec2(1.0,1.0)))

varying vec2 v_texcoord;
uniform sampler2D mytexture;
uniform float bloomThreshold = 0.4;
uniform vec2 pixshift;

void main(void) {
	vec4 c = (t(-2,2) + t(-1,2) + t(0,2) + t(1,2) + t(2,2) +
			 t(-2,1) + t(-1,1) + t(0,1) + t(1,1) + t(2,1) +
			 t(-2,0) + t(-1,0) + t(0,0) + t(1,0) + t(2,0) +
			 t(-2,-1) + t(-1,-1) + t(0,-1) + t(1,-1) + t(2,-1) +
			 t(-2,-2) + t(-1,-2) + t(0,-2) + t(1,-2) + t(2,-2))/10.0;
	gl_FragColor = c;
}