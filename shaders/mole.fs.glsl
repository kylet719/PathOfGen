#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int whacked;
uniform float anger_level;
uniform float time;

// Output color
layout(location = 0) out  vec4 color;

vec2 distort(vec2 uv, float freq_arg, float amp_arg, float speed_arg, bool death) 
{	
	// Find the edge which the px is closest to and divide by a decimal to increase the distorion factor accordingly
	float edgeFade = 0.2;
	float distortionFactor = min(min(uv[0], 1.0-uv[0]), min(uv[1], 1.0-uv[1])) / edgeFade;
	distortionFactor = clamp(distortionFactor, 0.0, 1.0);

	float frequency = freq_arg;
	float amplitude = amp_arg;
	float speed = speed_arg;

	// Simple oscillation equation: https://courses.lumenlearning.com/suny-osuniversityphysics/chapter/16-2-mathematics-of-waves/
    // Distortion factor for when close to an edge then don't distort (multiply by 0)
	float dx = sin(uv[0] * speed + time * frequency) * amplitude * distortionFactor;
	float dy = cos(uv[1] * speed + time * frequency) * amplitude * distortionFactor;


	if (death == true){
		uv[0] += dx;
		uv[1] += dy;
	}  else {
		uv[0] += dx;
	}
	return uv;
}

void main()
{
	if (whacked == 1){
		vec2 coord = distort(texcoord, 0.25, 0.3, 2, true); // On 
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(coord.x, coord.y));
		color[0] -= 0.35;
		color[1] += 0.15;
		color[2] += 0.251;
	} else {
		vec2 coord = distort(texcoord, anger_level, 0.05, 2, false); // On death
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(coord.x, coord.y));
	}
	
	
}
