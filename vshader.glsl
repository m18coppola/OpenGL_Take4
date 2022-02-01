#version 460

uniform mat4 v_mat;
uniform mat4 p_mat;
uniform float tf;
layout (location=0) in vec3 position;
layout (location=1) in vec2 texture_coord;
out vec2 tc;

mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main()
{

	float i = gl_InstanceID + tf;
	float a = sin(203.0 * i/8000.0) * 403.0;
	float b = cos(301.0 * i/4001.0) * 401.0;
	float c = sin(400.0 * i/6003.0) * 405.0;

	mat4 localRotX = buildRotateX(1.75*i);
	mat4 localRotY = buildRotateY(1.75*i);
	mat4 localRotZ = buildRotateZ(1.75*i);
	mat4 localTranslate = buildTranslate(a,b,c);

	mat4 m_mat = localTranslate * localRotX * localRotY * localRotZ;
	mat4 mv_mat = v_mat * m_mat;

	gl_Position = p_mat * mv_mat * (vec4(position, 1.0));
	tc = texture_coord;
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
