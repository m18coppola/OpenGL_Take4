#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

char * readShaderSource(char* shaderPath);
GLuint compileShader(GLenum type, char *source);
GLuint InitShader(char* vShaderPath, char* fShaderPath);
void printShaderLog(GLuint shader);
void printProgramLog(GLuint prog);

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
