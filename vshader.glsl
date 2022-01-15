#version 460

uniform mat4 v_mat;
uniform mat4 p_mat;
layout (location=0) in vec3 position;
layout (location=1) in vec2 texture_coord;
out vec2 tc;

void main()
{
	gl_Position = p_mat * v_mat * (vec4(position, 1.0));
	tc = texture_coord;
}

