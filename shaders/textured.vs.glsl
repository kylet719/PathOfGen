#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
layout (location = 2) in vec2 in_offset;


// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int is_instanced;

vec3 add_offset(vec3 pos)
{
	if (is_instanced == 1)
	{
		pos = vec3(pos.x + in_offset.x, pos.y - in_offset.y, pos.z);
	}

	return pos; 
}

void main()
{
	texcoord = in_texcoord;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	pos = add_offset(pos);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}