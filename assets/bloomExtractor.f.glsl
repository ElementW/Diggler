precision lowp float;

varying vec2 v_texcoord;
uniform sampler2D mytexture;
uniform float bloomThreshold; // = 0.4;

vec3 cap(vec3 val) {
	float highest = val.x;
	if (val.y > highest) highest = val.y;
	if (val.z > highest) highest = val.z;
	return val + vec3(1.0-highest);
}

void main(void) {
	vec4 c = texture2D(mytexture, v_texcoord);
	float colorSum = clamp( ((c.x + c.y + c.z) / 3.0 - bloomThreshold) / (1.0 - bloomThreshold), 0.0, 1.0);
	//gl_FragColor = vec4(c.rgb * colorSum, colorSum);
	gl_FragColor = vec4(cap(normalize(c.rgb * colorSum)), colorSum);
}