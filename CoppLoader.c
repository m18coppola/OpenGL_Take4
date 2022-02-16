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

//TODO: mem leak
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
