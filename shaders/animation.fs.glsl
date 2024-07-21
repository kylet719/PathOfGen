#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int frame_index;
uniform int sheet_width; 
uniform int frame_y;
uniform int sheet_height;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	float frame_width = 1 / float(sheet_width);
	float x_offset = float(frame_index) * frame_width;
	float frame_height = 1 / float(sheet_height);
	float y_offset = 1 - (float(frame_y) * frame_height);
	vec2 frame = vec2((texcoord.x * frame_width) + x_offset, (texcoord.y * frame_height) + y_offset);
	color = vec4(fcolor, 1.0) * texture(sampler0, frame);
}
