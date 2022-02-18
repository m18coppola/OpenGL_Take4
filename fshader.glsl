#version 430

in vec4 fPosition;
in vec3 fNormal;
in vec2 tc;

layout (binding=0) uniform sampler2D s;


out vec4 color;
void main()
{
	vec4 ambient = vec4(0.1, 0.1, 0.2, 1.0);
	vec3 light_pos = vec3(100.0, 100.0, 100.0);
	vec4 diffuse_level = vec4(0.7, 0.65, 0.65, 1.0);
	vec4 specular_level = vec4(0.7, 0.7, 0.7, 1.0);
	float shininess = 128;

	vec3 P = vec3(fPosition);
	vec3 N = normalize(fNormal);
	vec3 L = normalize(light_pos - P);
	vec3 R = reflect(-L, N);
	vec3 V = normalize(-P);
	
	vec4 diffuse = max(0.0, dot(N, L)) * diffuse_level * texture(s, tc);
	vec4 specular = pow(max(0.0, dot(R, V)), shininess) * specular_level;

	color = ambient + diffuse + specular;
}


