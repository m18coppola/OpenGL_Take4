#include <GL/glew.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#include "CoppLoader.h"

int
main(int argc, char *argv[])
{
	GLenum glewError;
	GLuint program;
	GLuint vao[1];
	GLuint vbo[1];
	float verts[] = {0.0,0.0,0.0,0.0,1.0,0.0,0.5,0.5,0.0,1.0,0.0,0.0};
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

	/* end of initialization */

	/* render*/
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	/* VAO represents a complete rendered object */
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	
	/* VBO is a allocation of graphics memory */
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	/* load data into vram */
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	/* game loop */
	running = 1;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = 0;
			}
		}
		/* render */
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 12);
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
