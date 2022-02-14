#include <cglm/cglm.h>
#include <GL/glew.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <SOIL2.h>

#include "CoppLoader.h"

#define LOOK_SENS 0.005
#define CAM_SPEED 0.5

#define WIDTH 1920
#define HEIGHT 1080

typedef struct {
	vec3 position;
	float pitch, yaw;
	mat4 perspective;
	int forward;
	int backward;
	int left;
	int right;
} Camera;

void
createViewMatrix(Camera c, mat4 vmat)
{
	vec3 dir = {0.0f, 0.0f, -1.0f};
	glm_vec3_rotate(dir, c.pitch, GLM_XUP);
	glm_vec3_rotate(dir, c.yaw, GLM_YUP);
	glm_look(c.position, dir, GLM_YUP, vmat);
}

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
	SDL_Window *window = SDL_CreateWindow("CoppEng", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(stderr, "Failed to create window. Exiting.\nSDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	/* capture mouse */
	SDL_SetRelativeMouseMode(SDL_TRUE);

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
		fprintf(stderr, "Error initializing GLEW. Exiting.\nGLEW_Error: %s\n", glewGetErrorString(glewError));
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

	/* setup camera */
	Camera cam = (Camera){
		.position = {0.0f, 2.0f, 8.0f},
		.pitch = 0.0f,
		.yaw = 0.0f,
		.forward = 0,
		.backward = 0,
		.left = 0,
		.right = 0
	};

	/* create a perspective projection matrix */
	glm_perspective(1.0f, 720.0/480.0, 0.1, 1000.0, cam.perspective);
	glUniformMatrix4fv(glGetUniformLocation(program, "p_mat"),
			1, GL_FALSE, (GLfloat *)cam.perspective);

	/* create a view matrix */
	mat4 v_mat;
	createViewMatrix(cam, v_mat);
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
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
	};

	/* VAO represents a complete rendered object */
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	/* VBO is a allocation of graphics memory */
	glGenBuffers(2, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_map), texture_map, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	/* set current texture (outside renderer, doesn't change for now) */
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	/* enable and generate mipmaps */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* use anisotropic filtering if supported */
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
		GLfloat aniso_setting = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso_setting);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso_setting);
	} else {
		printf("AF not supported!\n");
	}

	/* enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* game loop */
	running = 1;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			/* check for quit event */
			if (e.type == SDL_QUIT) {
				running = 0;
			}
			/* handle keyboard input */
			if (e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym){
					case SDLK_w:
						cam.forward = 1;
						break;
					case SDLK_a:
						cam.left = 1;
						break;
					case SDLK_d:
						cam.right = 1;
						break;
					case SDLK_s:
						cam.backward = 1;
						break;
				}
			} else if (e.type == SDL_KEYUP) {
				switch(e.key.keysym.sym){
					case SDLK_w:
						cam.forward = 0;
						break;
					case SDLK_a:
						cam.left = 0;
						break;
					case SDLK_d:
						cam.right = 0;
						break;
					case SDLK_s:
						cam.backward = 0;
						break;
				}
			}
		}
		/* rotate cube */
		for (int i = 0; i < 108; i+= 3) {
			glm_vec3_rotate(&verts[i], 0.01f, (vec3){0.0, 1.0, 0.0});
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		/* rotate player */
		int mousex, mousey;
		SDL_GetRelativeMouseState(&mousex, &mousey);
		cam.yaw -= (float)mousex * LOOK_SENS;
		cam.pitch -= (float)mousey * LOOK_SENS;
		/* move player */
		vec3 velocity = {0.0f, 0.0f, 0.0f};
		if (cam.forward) {
			velocity[2] += -CAM_SPEED;
		}
		if (cam.backward) {
			velocity[2] += CAM_SPEED;
		}
		if (cam.left) {
			velocity[0] += -CAM_SPEED;
		}
		if (cam.right) {
			velocity[0] += CAM_SPEED;
		}
		glm_vec3_rotate(velocity, cam.yaw, GLM_YUP);
		glm_vec3_add(cam.position, velocity, cam.position);
		/* update view matrix */
		createViewMatrix(cam, v_mat);
		glUniformMatrix4fv(
				glGetUniformLocation(program, "v_mat"),
				1,
				GL_FALSE,
				(GLfloat *)v_mat
		);
		/* render */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1);
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
