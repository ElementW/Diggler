#define PI 3.1415926535897932384626433832795
uniform mat4 mvp;
attribute vec4 coord;

#ifdef COLORED
attribute vec4 color;
varying vec4 v_color;
#endif
#ifdef TEXTURED
attribute vec2 texcoord;
varying vec2 v_texcoord;
#endif
#ifdef TIME
uniform float time;
#endif
#ifdef WAVE
attribute float wave;
#endif

void main(void) {
#ifdef COLORED
	v_color = color;
#endif
#ifdef TEXTURED
	v_texcoord = texcoord;
#endif
	vec3 coord = coord.xyz;
#ifdef WAVE
	float yShift = sin(time+(coord.x+coord.z)/16.0*6*PI)*(wave) - wave;
	coord.y += yShift;
	//v_texcoord.y -= yShift/8.0;
#endif
	gl_Position = mvp * vec4(coord, 1);
}