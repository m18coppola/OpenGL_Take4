#version 460

uniform mat4 v_mat;
uniform mat4 p_mat;

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tcs;

out vec3 fPosition;
out vec3 fNormal;
out vec3 light_dir;
out vec2 tc;

void main()
{
	vec3 light_pos = vec3(100.0, 100.0, 100.0);

	gl_Position = p_mat * v_mat * (vec4(position, 1.0));
	fPosition = (v_mat * vec4(position, 1.0)).xyz;
	fNormal = (transpose(inverse(v_mat)) * vec4(normal, 1.0)).xyz;
	light_dir = (v_mat * vec4(light_pos, 1.0)).xyz - fPosition;
	tc = tcs;
}
