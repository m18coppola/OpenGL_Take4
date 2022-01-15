#include <cglm/cglm.h>
#include <GL/glew.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <SOIL2.h>

#include "CoppLoader.h"

int
main(int argc, char *argv[])
{
	GLenum glewError;
	GLuint program;
	GLuint vao[1];
	GLuint vbo[2];
	SDL_Event e;
	int running;

	/* begin SDL2 */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL2 failed to initialize. Exiting.\nSDL_Error: %s\n", SDL_GetError());
		exit(-1);
	}

	/* set openGL version */
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	/* create window */
	SDL_Window *window = SDL_CreateWindow("CoppEng", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 720, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(stderr, "Failed to create window. Exiting.\nSDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	/* create graphics context */
	SDL_GLContext g_context = SDL_GL_CreateContext(window);
	if (g_context == NULL) {
		fprintf(stderr, "Failed to create graphics context. Exiting.\nSDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	/* initialize GLEW */
	glewExperimental = GL_TRUE;
	glewError = glewInit();
	if (glewError != GLEW_OK) {
		fprintf(stderr, "Error initializing Glew. Exiting.\nGLEW_Error: %s\n", glewGetErrorString(glewError));
		SDL_Quit();
		exit(-1);
	}

	/* set clear color of renderer */
	glClearColor(1.0, 1.0, 1.0, 1.0);

	/* attach shader to GPU */
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	/* load crate texture */
	GLuint texture_id = SOIL_load_OGL_texture("crate.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (texture_id == 0) {
		fprintf(stderr, "SOIL2 failed to load crate.jpg as OGL texture. Exiting.\n");
		exit(-1);
	}

	/* create a perspective projection matrix */
	mat4 p_mat;
	glm_perspective(1.0f, 720.0/480.0, 0.1, 1000.0, p_mat);
	glUniformMatrix4fv(glGetUniformLocation(program, "p_mat"),
			1, GL_FALSE, (GLfloat *)p_mat);

	/* create a view matrix */
	mat4 v_mat;
	glm_mat4_identity(v_mat);
	glm_translate(v_mat, (vec3){0.0f, -2.0f, -8.0f});
	glUniformMatrix4fv(glGetUniformLocation(program, "v_mat"),
			1, GL_FALSE, (GLfloat *)v_mat);
	
	/* load data into vram */
	float verts[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	float texture_map[] = {
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 1.0, 0.0, 1.0, 1.0
	};

	/* VAO represents a complete rendered object */
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	/* VBO is a allocation of graphics memory */
	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_map), texture_map, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

	
	/* game loop */
	running = 1;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = 0;
			}
		}
		/* rotate cube */
		for (int i = 0; i < 108; i+= 3) {
			glm_vec3_rotate(&verts[i], 0.01f, (vec3){0.0, 1.0, 0.0});
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		/* render */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		/* swap buffers */
		SDL_GL_SwapWindow(window);
	}

	/* clean up */
	SDL_DestroyWindow(window);
	window = NULL;
	glDeleteProgram(program);
	SDL_Quit();
	return 0;
}
