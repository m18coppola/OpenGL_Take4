#version 460

uniform mat4 v_mat;
uniform mat4 p_mat;
uniform float tf;
layout (location=0) in vec3 position;
layout (location=1) in vec2 texture_coord;
out vec2 tc;
out vec4 varyingColor;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);
layout (binding=0) uniform sampler2D s;

void main()
{
	gl_Position = p_mat * v_mat * (vec4(position, 1.0));
	tc = texture_coord;
	varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}

mat4 buildTranslate(float x, float y, float z)
{	mat4 trans = mat4(	1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		x, y, z, 1.0 );
	return trans;
}

//  rotation around the X axis
mat4 buildRotateX(float rad)
{	mat4 xrot = mat4(	1.0, 0.0, 0.0, 0.0,
		0.0, cos(rad), -sin(rad), 0.0,
		0.0, sin(rad), cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return xrot;
}

//  rotation around the Y axis
mat4 buildRotateY(float rad)
{	mat4 yrot = mat4(	cos(rad), 0.0, sin(rad), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(rad), 0.0, cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return yrot;
}

//  rotation around the Z axis
mat4 buildRotateZ(float rad)
{	mat4 zrot = mat4(	cos(rad), sin(rad), 0.0, 0.0,
		-sin(rad), cos(rad), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return zrot;
}
