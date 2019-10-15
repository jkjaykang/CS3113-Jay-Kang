#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include "Text.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define PLATFORM_COUNT 50
#define GOAL_COUNT 2

struct GameState {
	Entity player;
	Entity platforms[PLATFORM_COUNT];
	Entity goal[GOAL_COUNT];
	Text gameText;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}



void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Physics!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	state.player.entityType = PLAYER;
	state.player.isStatic = false;

	state.player.position = glm::vec3(0, 4, 0);
	state.player.acceleration = glm::vec3(0, -0.1f, 0);
	state.player.velocity = glm::vec3(0, 0, 0);
	state.player.textureID = LoadTexture("pod.png");

	GLuint tileTextureID = LoadTexture("tile.png");

	for (int i = 0; i < 15; i++)
	{
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-7.0f + i, -5.75f, 0);
	}
	for (int i = 15; i < 27; i++)
	{
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(7.0f, -19.75f + i, 0);
	}
	for (int i = 27; i < 39; i++)
	{
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-7.0f, -31.75f + i, 0);
	}
	for (int i = 39; i < 45; i++)
	{
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-45.0f + i, 2.0f, 0);
	}
	for (int i = 45; i < PLATFORM_COUNT; i++)
	{
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-45.0f + i, -1.0f, 0);
	}


	GLuint goalTextureID = LoadTexture("goal.png");
	state.goal[0].entityType = GOAL;
	state.goal[0].textureID = goalTextureID;
	state.goal[0].position = glm::vec3(-2.0f, -5.6f, 0);
	state.goal[1].entityType = GOAL;
	state.goal[1].textureID = goalTextureID;
	state.goal[1].position = glm::vec3(-1.0f, -5.6f, 0);

	GLuint wordTextureID = LoadTexture("font.png");
	state.gameText.fontTextureID = wordTextureID;

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-7.0f, 7.0f, -5.75f, 5.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				if (state.player.collidedBottom)
				{
					//state.player.Jump();;
				}
				break;

			}
			break;
		}
	}

	state.player.velocity.x = 0;

	// Check for pressed/held keys below
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A] && state.player.won == 0)
	{
		state.player.acceleration.x += -0.05f;
	}
	else if (keys[SDL_SCANCODE_D] && state.player.won == 0)
	{
		state.player.acceleration.x += 0.05f;
	}

}




#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
		state.player.Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
		state.player.Update(FIXED_TIMESTEP, state.goal, GOAL_COUNT);
		if (state.player.won == 1)
		{
			state.gameText.position = state.gameText.position = glm::vec3(-4.3f, 0, 0);
			state.gameText.DrawText(&program, state.gameText.fontTextureID, "Mission Failed", 0.5, 0.15, state.gameText.position);
		}
		else if (state.player.won == 2)
		{
			state.gameText.position = state.gameText.position = glm::vec3(-5.1f, 0, 0);
			state.gameText.DrawText(&program, state.gameText.fontTextureID, "Mission Succesful", 0.5, 0.15, state.gameText.position);

		}
		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	state.player.Render(&program);

	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		state.platforms[i].Render(&program);
	}

	state.goal[0].Render(&program);
	state.goal[1].Render(&program);

	state.gameText.Render(&program);


	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}
