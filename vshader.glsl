#version 460

uniform mat4 v_mat;
uniform mat4 p_mat;

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tcs;

out vec4 fPosition;
out vec3 fNormal;
out vec2 tc;

void main()
{
	gl_Position = p_mat * v_mat * (vec4(position, 1.0));
	fPosition = v_mat * vec4(position, 1.0);
	fNormal = normal;
	tc = tcs;
}
