#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>

#include "CoppLoader.h"

char * readShaderSource(char* shaderPath);
GLuint compileShader(GLenum type, char *source);
GLuint InitShader(char* vShaderPath, char* fShaderPath);
void printShaderLog(GLuint shader);
void printProgramLog(GLuint prog);

Model *
readOBJ(char *OBJpath)
{
	FILE *fp;
	char *buffer = NULL;
	size_t buffer_size;
	ssize_t line_size;
	int v_count, n_count, t_count, p_count;
	float **vert_vecs;
	float **norm_vecs;
	float **tcs_vecs;
	int *v_index;
	int *n_index;
	int *t_index;
	int i;
	Model *model;

	fp = fopen(OBJpath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open .obj file: %s\n", OBJpath);
		exit(-1);
	}

	/* count data and allocate an OBJ buffer */
	v_count = 0;
	n_count = 0;
	t_count = 0;
	p_count = 0;
	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			switch (buffer[1]) {
				case ' ':
					v_count++;
					break;
				case 'n':
					n_count++;
					break;
				case 't':
					t_count++;
					break;
			}
		} else if (buffer[0] == 'f') {
			p_count++;
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);
	buffer = NULL;
	rewind(fp);

	/* prepare model */
	model = malloc(sizeof(Model));
	/* 3 floats per 3 vecs per poly */
	model->verts_size = 3 * 3 * p_count;
	model->normals_size = 3 * 3 * p_count;
	/* 2 floats per 3 vecs per poly */
	model->tcs_size = 2 * 3 * p_count;
	model->verts = malloc(sizeof(float) * model->verts_size);
	model->normals = malloc(sizeof(float) * model->normals_size);
	model->tcs = malloc(sizeof(float) * model->tcs_size);

	vert_vecs = malloc(sizeof(float) * 3 * v_count);
	tcs_vecs = malloc(sizeof(float) * 2 * t_count);
	norm_vecs = malloc(sizeof(float) * 3 * n_count);
	v_index = malloc(sizeof(int) * p_count * 3);
	n_index = malloc(sizeof(int) * p_count * 3);
	t_index = malloc(sizeof(int) * p_count * 3);

	int vi, ni, ti;
	int vii, nii, tii;
	vi = 0;
	ni = 0;
	ti = 0;
	vii = 0;
	nii = 0;
	tii = 0;
	char *token_buffer;
	float x, y, z;
	i = 0;

	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			token_buffer = buffer+2;
			switch (buffer[1]) {
				case ' ':
					vert_vecs[vi] = malloc(sizeof(float) * 3);
					sscanf(token_buffer, "%f %f %f", &x, &y, &z);
					vert_vecs[vi][0] = x;
					vert_vecs[vi][1] = y;
					vert_vecs[vi][2] = z;
					vi++;
					break;
				case 'n':
					norm_vecs[ni] = malloc(sizeof(float) * 3);
					sscanf(token_buffer, "%f %f %f", &x, &y, &z);
					norm_vecs[ni][0] = x;
					norm_vecs[ni][1] = y;
					norm_vecs[ni][2] = z;
					ni++;
					break;
				case 't':
					tcs_vecs[ti] = malloc(sizeof(float) * 2);
					sscanf(token_buffer, "%f %f", &x, &y);
					tcs_vecs[ti][0] = x;
					tcs_vecs[ti][1] = y;
					ti++;
					break;
			}

		} else if (buffer[0] == 'f') {
			sscanf(
				token_buffer,
				"%d/%d/%d %d/%d/%d %d/%d/%d",
				&v_index[vii], &t_index[tii], &n_index[nii],
				&v_index[vii+1], &t_index[tii+1], &n_index[nii+1],
				&v_index[vii+2], &t_index[tii+2], &n_index[nii+2]
			);
			vii+=3;
			tii+=3;
			nii+=3;
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);
	buffer = NULL;
	fclose(fp);

	//printf("v %f %f %f\n", vert_vecs[i][0], vert_vecs[i][1], vert_vecs[i][2]);


	vii = 0;
	nii = 0;
	tii = 0;
	for(i = 0; i < p_count * 3; i++) {
		model->verts[vii] = vert_vecs[v_index[i]-1][0];
		model->verts[vii + 1] = vert_vecs[v_index[i]-1][1];
		model->verts[vii + 2] = vert_vecs[v_index[i]-1][2];
		vii+=3;

		model->normals[nii] = norm_vecs[n_index[i]-1][0];
		model->normals[nii + 1] = norm_vecs[n_index[i]-1][1];
		model->normals[nii + 2] = norm_vecs[n_index[i]-1][2];
		nii+=3;

		model->tcs[tii] = tcs_vecs[t_index[i]-1][0];
		model->tcs[tii + 1] = tcs_vecs[t_index[i]-1][1];
		tii+=2;
	}
	return model;

}
#if 0
Model *
readOBJ(char* OBJpath) {
	FILE *fp;
	char *buffer = NULL;
	size_t buffer_size;
	ssize_t line_size;
	int v_count, n_count, t_count, p_count;
	vec3 *vert_vecs;
	vec3 *norm_vecs;
	vec3 *tcs_vecs;
	int vi = 0;
	int ni = 0;
	int ti = 0;
	int elem_count;
	char *token_buffer;
	char *endptr;
	float **dest;
	int *ip;
	int i;
	Model *model;

	fp = fopen(OBJpath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open .obj file: %s\n", OBJpath);
		exit(-1);
	}

	/* count data and allocate an OBJ buffer */
	v_count = 0;
	n_count = 0;
	t_count = 0;
	p_count = 0;
	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			switch (buffer[1]) {
				case ' ':
					v_count++;
					break;
				case 'n':
					n_count++;
					break;
				case 't':
					t_count++;
					break;
			}
		} else if (buffer[0] == 'f') {
			p_count++;
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);
	buffer = NULL;
	rewind(fp);

	/* read compressed data into temp buffers */
	vert_vecs = malloc(sizeof(float) * 3 * v_count);
	norm_vecs = malloc(sizeof(float) * 3 * n_count);
	tcs_vecs = malloc(sizeof(float) * 2 * t_count);
	int *verti_buffer;
	int *normi_buffer;
	int *tcsi_buffer;
	int vii;
	int nii;
	int tii;

	/* prepare model */
	model = malloc(sizeof(Model));
	/* 3 floats per 3 vecs per poly */
	model->verts_size = 3 * 3 * p_count;
	model->normals_size = 3 * 3 * p_count;
	/* 2 floats per 3 vecs per poly */
	model->tcs_size = 2 * 3 * p_count;
	model->verts = malloc(sizeof(float) * model->verts_size);
	model->normals = malloc(sizeof(float) * model->normals_size);
	model->tcs = malloc(sizeof(float) * model->tcs_size);

	verti_buffer = malloc(sizeof(int) * p_count * 3);
	normi_buffer = malloc(sizeof(int) * p_count * 3);
	tcsi_buffer = malloc(sizeof(int) * p_count * 3);
	vii = 0;
	nii = 0;
	tii = 0;
	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			switch (buffer[1]) {
				case ' ':
					dest = &vert_vecs;
					elem_count = 3;
					ip = &vi;
					break;
				case 'n':
					dest = &norm_vecs;
					elem_count = 3;
					ip = &ni;
					break;
				case 't':
					dest = &tcs_vecs;
					elem_count = 2;
					ip = &ti;
					break;
				default:
					dest = NULL;
					elem_count = 0;
					ip = NULL;
					break;
			}
			token_buffer = buffer+2;
			for (i = 0; i < elem_count; i++) {
					(*dest)[*ip] = strtof(token_buffer, &endptr);
					(*ip)++;
					token_buffer = endptr;
			}

		} else if (buffer[0] == 'f') {
			token_buffer = buffer+1;
			int num;
			for (i = 0; i < 3; i++) {
				num = strtol(token_buffer, &endptr, 10);
				token_buffer = endptr;
				if(token_buffer[0] == '/') token_buffer++;
				verti_buffer[vii] = num - 1;
				vii++;
				model->verts[(vii*3) + 0] = vert_vecs[(num-1) * 3 + 0];
				model->verts[(vii*3) + 1] = vert_vecs[(num-1) * 3 + 1];
				model->verts[(vii*3) + 2] = vert_vecs[(num-1) * 3 + 2];

				num = strtol(token_buffer, &endptr, 10);
				token_buffer = endptr;
				if(token_buffer[0] == '/') token_buffer++;
				tcsi_buffer[tii] = num - 1;
				tii++;

				num = strtol(token_buffer, &endptr, 10);
				token_buffer = endptr;
				if(token_buffer[0] == '/') token_buffer++;
				normi_buffer[nii] = num - 1;
				nii++;
			}
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);
	buffer = NULL;
	fclose(fp);

	for (i = 0; i < v_count; i++) {
		printf("v  %f, %f, %f\n", vert_vecs[3 * i], vert_vecs[(3 * i) + 1], vert_vecs[(3 * i) + 2]);
	}
	for (i = 0; i < n_count; i++) {
		printf("vn %f, %f, %f\n", norm_vecs[3 * i], norm_vecs[(3 * i) + 1], norm_vecs[(3 * i) + 2]);
	}
	for (i = 0; i < t_count; i++) {
		printf("vt %f, %f\n", tcs_vecs[2 * i], tcs_vecs[(2 * i) + 1]);
	}
	for (i = 0; i < p_count; i++) {
		printf(
			"f %d/%d/%d %d/%d/%d %d/%d/%d\n",
			verti_buffer[(i * 3)],
			tcsi_buffer[(i * 3)],
			normi_buffer[(i * 3)],

			verti_buffer[(i * 3) + 1],
			tcsi_buffer[(i * 3) + 1],
			normi_buffer[(i * 3) + 1],

			verti_buffer[(i * 3) + 2],
			tcsi_buffer[(i * 3) + 2],
			normi_buffer[(i * 3) + 2]
		);
	}


	vi = 0;
	ni = 0;
	ti = 0;
	for (i = 0; 0 && i < p_count; i++) {
		model->verts[(i*9) + 0] = vert_vecs[verti_buffer[(i * 3) + 0] + 0];
		model->verts[(i*9) + 1] = vert_vecs[verti_buffer[(i * 3) + 0] + 1];
		model->verts[(i*9) + 2] = vert_vecs[verti_buffer[(i * 3) + 0] + 2];

		model->verts[(i*9) + 3] = vert_vecs[verti_buffer[(i * 3) + 1] + 0];
		model->verts[(i*9) + 4] = vert_vecs[verti_buffer[(i * 3) + 1] + 1];
		model->verts[(i*9) + 5] = vert_vecs[verti_buffer[(i * 3) + 1] + 2];

		model->verts[(i*9) + 6] = vert_vecs[verti_buffer[(i * 3) + 2] + 0];
		model->verts[(i*9) + 7] = vert_vecs[verti_buffer[(i * 3) + 2] + 1];
		model->verts[(i*9) + 8] = vert_vecs[verti_buffer[(i * 3) + 2] + 2];
	}
	return model;
}

Model *
readOBJ(char* OBJpath) {
	FILE *fp;
	Model *model;
	char *buffer = NULL;
	size_t buffer_size;
	ssize_t line_size;
	int elem_count;
	FloatNode **dest;
	FloatNode *top;
	FloatNode *top_old;
	FloatNode *vert_buf = NULL;
	FloatNode *tcs_buf = NULL;
	FloatNode *norm_buf = NULL;
	FloatNode *vi_buf = NULL;
	int i;
	float left, right, elem;
	char *number_buf;
	char *endptr;
	float *uverts;
	float *unormals;
	float *utcs;
	int uverts_size = 0;
	int unorms_size = 0;
	int utcs_size = 0;
	int *face_index;
	int face_index_size = 0;

	model = malloc(sizeof(Model));
	model->verts_size = 0;
	model->normals_size = 0;
	model->tcs_size = 0;

	fp = fopen(OBJpath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open .obj file: %s\n", OBJpath);
		exit(-1);
	}

	vec3 vert_vecs[1000];
	vec3 norm_vecs[1000];
	vec2 tcs_vecs[1000];

	/* read file into buffers */
	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			/* determine number of elements and destination */
			switch (buffer[1]) {
				case ' ':
					elem_count = 3;
					dest = &vert_buf;
					uverts_size += 3;
					break;
				case 'n':
					elem_count = 3;
					dest = &norm_buf;
					unorms_size += 3;
					break;
				case 't':
					elem_count = 2;
					dest = &tcs_buf;
					utcs_size += 2;
					break;
				default:
					elem_count = 0;
					dest = NULL;
				
			}
			top = *dest;

			/* push floats into buffer */
			number_buf = buffer+2;
			for (i = 0; i < elem_count; i++) {
					top_old = top;
					top = malloc(sizeof(FloatNode));
					top->val = strtof(number_buf, &endptr);
					number_buf = endptr;
					top->next = top_old;
			}
			*dest = top;
		} else if (buffer[0] == 'f') {
			number_buf = buffer+1;
			elem_count = 9;
			dest = &vi_buf;
			face_index_size += 9;
			top = *dest;
			for (i = 0; i < elem_count; i++) {
				if (i%3 == 0) {
					top_old = top;
					top = malloc(sizeof(FloatNode));
					top->val = strtol(number_buf, &endptr, 10);
					number_buf = endptr;
					if (number_buf[0] == '/') number_buf++;
					top->next = top_old;
				} else {
					strtol(number_buf, &endptr, 10);
					number_buf = endptr;
					if (number_buf[0] == '/') number_buf++;
				}
			}
			*dest = top;
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);

	/* dump buffers into addressable arrays */
	uverts = malloc(sizeof(float) * uverts_size);
	i = uverts_size - 1;
	while (vert_buf != NULL) {
		uverts[i] = vert_buf->val;
		i--;

		top_old = vert_buf;
		vert_buf = vert_buf->next;
		free(top_old);
	}
	
	utcs = malloc(sizeof(float) * utcs_size);
	i = utcs_size - 1;
	while (tcs_buf != NULL) {
		utcs[i] = tcs_buf->val;
		i--;

		top_old = tcs_buf;
		tcs_buf = tcs_buf->next;
		free(top_old);
	}
	
	unormals = malloc(sizeof(float) * unorms_size);
	i = unorms_size - 1;
	while (norm_buf != NULL) {
		unormals[i] = norm_buf->val;
		i--;

		top_old = norm_buf;
		norm_buf = norm_buf->next;
		free(top_old);
	}

	face_index = malloc(sizeof(float) * face_index_size);
	i = face_index_size - 1;
	while (vi_buf != NULL) {
		face_index[i] = vi_buf->val;
		i--;

		top_old = vi_buf;
		vi_buf = vi_buf->next;
		free(top_old);
	}

	/* parse faces and load into model */
	model->verts = malloc(sizeof(float) * face_index_size);
	model->normals = malloc(sizeof(float) * face_index_size);
	model->tcs = malloc(sizeof(float) * (face_index_size/3) * 2);
	int vcount = 0;
	int ncount = 0;
	int tccount = 0;
	for(i = 0; i < face_index_size; i+=3) {
		model->verts[vcount] = uverts[face_index[i] * 3];
		vcount++;
		model->verts[vcount] = uverts[(face_index[i] * 3) + 1];
		vcount++;
		model->verts[vcount] = uverts[(face_index[i] * 3) + 2];
		vcount++;


		model->tcs[tccount] = utcs[face_index[i+1] * 2];
		tccount++;
		model->tcs[tccount] = utcs[face_index[i+1] * 2 + 1];
		tccount++;

		model->normals[ncount] = unormals[face_index[i+2] * 3];
		ncount++;
		model->normals[ncount] = unormals[face_index[i+2] * 3 + 1];
		ncount++;
		model->normals[ncount] = unormals[face_index[i+2] * 3 + 2];
		ncount++;
	}
	model->verts_size = vcount;
	model->normals_size = ncount;
	model->tcs_size = tccount;

	return model;
}
#endif

void
destroyModel(Model **model_ptr) {
	Model *model = *model_ptr;
	free(model->verts);
	free(model->tcs);
	free(model->normals);
	free(model->v_index);
	free(model);
	*model_ptr = NULL;
}

char *
readShaderSource(char* shaderPath)
{
    char *buffer = 0;
    long length;
    FILE *fp;

    fp = fopen(shaderPath, "r");
    if (fp == NULL) {
        printf("Failed to open file: %s\n", shaderPath);
    }
    fseek (fp, 0, SEEK_END);
    length = ftell (fp);
    fseek (fp, 0, SEEK_SET);
    buffer = malloc (length + 1);
    if (!buffer)  {
        printf("Failed to allocate memory for source!");
    }
    fread (buffer, 1, length, fp);
    fclose (fp);

    buffer[length] = '\0';
    return buffer;
}

GLuint
compileShader(GLenum type, char *sourcePath)
{
    GLuint s = glCreateShader(type);
    char *source = readShaderSource(sourcePath);
    glShaderSource(s, 1, (const GLchar * const *)&source, NULL);

    glCompileShader(s);
    GLint status;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char errlog[256];
        glGetShaderInfoLog(s, 512, NULL, errlog);
        printf("Failed to compile\n%s:\n%s\n", source, errlog);
    }
    free(source);
    return s;
}

GLuint
InitShader(char* vShaderPath, char* fShaderPath)
{
    GLuint vShader = compileShader(GL_VERTEX_SHADER, vShaderPath);
    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fShaderPath);

    GLuint sp = glCreateProgram();
    glAttachShader(sp, vShader);
    glAttachShader(sp, fShader);
    glLinkProgram(sp);

    GLint linked;
    glGetProgramiv(sp, GL_LINK_STATUS, &linked);
    if (!linked) {
        char errlog[256];
        glGetProgramInfoLog(sp, 512, NULL, errlog);
        printf("Shader program failed to link:\n%s", errlog);
    }

    return sp;
}

void
printShaderLog(GLuint shader)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		printf("Shader Info Log:");
		printf(log);
		printf("\n");
		free(log);
	}
}

void
printProgramLog(GLuint prog)
{
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH,&len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		printf("Program Info Log:");
		printf(log);
		printf("\n");
		free(log);
	}
}

int
checkOpenGLError()
{
	int foundError = 0;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		printf("glError: %d\n", glErr);
		foundError = 1;
		glErr = glGetError();
	}
	return foundError;
}
