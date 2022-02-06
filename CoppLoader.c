#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include "CoppLoader.h"

char * readShaderSource(char* shaderPath);
GLuint compileShader(GLenum type, char *source);
GLuint InitShader(char* vShaderPath, char* fShaderPath);
void printShaderLog(GLuint shader);
void printProgramLog(GLuint prog);

enum OBJ_DATATYPE {V, VT, VN, F, IGN};

typedef struct float_node {
	float val;
	struct float_node *next;
} FloatNode;

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

	model = malloc(sizeof(Model));
	model->verts_size = 0;
	model->tcs_size = 0;
	model->normals_size = 0;
	model->vi_size = 0;

	fp = fopen(OBJpath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open .obj file: %s\n", OBJpath);
		exit(-1);
	}

	/* read file into buffers */
	line_size = getline(&buffer, &buffer_size, fp);
	while (line_size >= 0) {
		if (buffer[0] == 'v') {
			/* determine number of elements and destination */
			switch (buffer[1]) {
				case ' ':
					elem_count = 3;
					dest = &vert_buf;
					model->verts_size += 3;
					break;
				case 'n':
					elem_count = 3;
					dest = &norm_buf;
					model->normals_size += 3;
					break;
				case 't':
					elem_count = 2;
					dest = &tcs_buf;
					model->tcs_size += 2;
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
			model->vi_size += 9;
			top = *dest;
			for (i = 0; i < elem_count; i++) {
				top_old = top;
				top = malloc(sizeof(FloatNode));
				top->val = strtol(number_buf, &endptr, 10);
				number_buf = endptr;
				if (number_buf[0] == '/') number_buf++;
				top->next = top_old;
			}
			*dest = top;
		}
		line_size = getline(&buffer, &buffer_size, fp);
	}
	free(buffer);

	/* dump buffers into model */
	model->verts = malloc(sizeof(float) * model->verts_size);
	i = model->verts_size - 1;
	while (vert_buf != NULL) {
		model->verts[i] = vert_buf->val;
		i--;

		top_old = vert_buf;
		vert_buf = vert_buf->next;
		free(top_old);
	}
	
	model->tcs = malloc(sizeof(float) * model->tcs_size);
	i = model->tcs_size - 1;
	while (tcs_buf != NULL) {
		model->tcs[i] = tcs_buf->val;
		i--;

		top_old = tcs_buf;
		tcs_buf = tcs_buf->next;
		free(top_old);
	}
	
	model->normals = malloc(sizeof(float) * model->normals_size);
	i = model->normals_size - 1;
	while (norm_buf != NULL) {
		model->normals[i] = norm_buf->val;
		i--;

		top_old = norm_buf;
		norm_buf = norm_buf->next;
		free(top_old);
	}

	model->v_index = malloc(sizeof(float) * model->vi_size);
	i = model->vi_size - 1;
	while (vi_buf != NULL) {
		model->v_index[i] = vi_buf->val;
		i--;

		top_old = vi_buf;
		vi_buf = vi_buf->next;
		free(top_old);
	}

	for (i = 0; i < model->tcs_size; i++) {
		printf("%f\n", model->tcs[i]);
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
