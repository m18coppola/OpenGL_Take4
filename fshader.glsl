#version 430

in vec3 fPosition;
in vec3 fNormal;
in vec3 light_dir;
in vec2 tc;

layout (binding=0) uniform sampler2D s;


out vec4 color;
void main()
{
	vec4 ambient = vec4(0.1, 0.1, 0.2, 1.0);
	vec4 diffuse_level = vec4(0.7, 0.65, 0.65, 1.0);
	vec4 specular_level = vec4(0.7, 0.7, 0.7, 1.0);
	float shininess = 128;
	vec4 myColor = vec4(0.8, 0.2, 0.1, 1.0);

	
	vec3 L = normalize(light_dir);
	vec3 N = normalize(fNormal);
	vec3 V = normalize(-fPosition);

	vec3 R = normalize(reflect(-L, N));
	float cosTheta = dot(L, N);
	float cosPhi = dot(V, R);
	
	vec3 diffuse = diffuse_level.xyz * max(cosTheta, 0.0) * texture(s, tc).xyz;
	vec3 specular = specular_level.xyz * pow(max(cosPhi, 0.0), shininess);

	color = vec4(ambient.xyz + diffuse + specular, 1.0);
}


