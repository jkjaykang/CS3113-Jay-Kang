#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#include "Ball.h"
#include "Wall.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool noWinner = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

float lastTicks = 0;

Entity player1;
Entity player2;
Ball pong;
Wall top;
Wall bottom;

Mix_Music* music;
Mix_Chunk* bounce;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	music = Mix_LoadMUS("pong.mp3");
	Mix_PlayMusic(music, -1);

	bounce = Mix_LoadWAV("bounce.wav");


#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

	player1.position = glm::vec3(-4.8f, 0.0f, 0.0f);
	player2.position = glm::vec3(4.8f, 0.0f, 0.0f);
	top.position = glm::vec3(0.0f, 3.75f, 0.0f);
	bottom.position = glm::vec3(0.0f, -3.75f, 0.0f);
	pong.position = glm::vec3(0);
	
	player1.speed = 4;
	player2.speed = 4;
	pong.speed = 3.5;

	pong.movement = glm::vec3(-1.0f, 0.2f, 0.0f);
	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
	player1.movement = glm::vec3(0, 0, 0);
	player2.movement = glm::vec3(0, 0, 0);

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
				// Some sort of action
				break;

			}
			break;
		}
	}

	// Check for pressed/held keys below
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_W] && noWinner == true)
	{
		player1.movement.y = 1;
	}
	else if (keys[SDL_SCANCODE_S] && noWinner == true)
	{
		player1.movement.y = -1;
	}

	if (keys[SDL_SCANCODE_UP] && noWinner == true)
	{
		player2.movement.y = 1;
	}
	else if (keys[SDL_SCANCODE_DOWN] && noWinner == true)
	{
		player2.movement.y = -1;
	}
	
}

void checkCollidePaddle(float x1, float x2, float w1, float w2, float y1, float y2, float h1, float h2)
{
	if (abs(pong.position.x) >= 4.8f)
	{
		noWinner = false;
		pong.movement.x = 0;
		pong.movement.y = 0;
	}
	
	float xdist = fabs(x2 - x1) - ((w1 + w2) / 2.0f);
	float ydist = fabs(y2 - y1) - ((h1 + h2) / 2.0f);

	if (xdist < 0 && ydist < 0)
	{
		Mix_PlayChannel(-1, bounce, 0);
		if (pong.movement.x > 0)
		{
			pong.movement.x = pong.movement.x - 2;
		}
		else if (pong.movement.x < 0)
		{
			pong.movement.x = pong.movement.x + 2;
		}

		int random = rand() % 2;
		if (random > 0)
		{
			if (pong.movement.y > 0)
			{
				pong.movement.y = pong.movement.y - 0.4;
			}
			else if (pong.movement.y < 0)
			{
				pong.movement.y = pong.movement.y + 0.4;
			}
		}
	}
}

void checkCollideWall(float x1, float x2, float w1, float w2, float y1, float y2, float h1, float h2)
{
	float xdist = fabs(x2 - x1) - ((w1 + w2) / 2.0f);
	float ydist = fabs(y2 - y1) - ((h1 + h2) / 2.0f);

	if (xdist < 0 && ydist < 0)
	{
		Mix_PlayChannel(-1, bounce, 0);
		if (pong.movement.y > 0)
		{
			pong.movement.y = pong.movement.y - 0.4;
		}
		else if (pong.movement.y < 0)
		{
			pong.movement.y = pong.movement.y + 0.4;
		}
	}
}



void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	checkCollidePaddle(player1.position.x, pong.position.x, 0.4, 0.4, player1.position.y, pong.position.y, 1.6, 0.4);
	checkCollidePaddle(player2.position.x, pong.position.x, 0.4, 0.4, player2.position.y, pong.position.y, 1.6, 0.4);
	checkCollideWall(top.position.x, pong.position.x, 10.0, 0.4, top.position.y, pong.position.y, 0.4, 0.4);
	checkCollideWall(bottom.position.x, pong.position.x, 10.0, 0.4, bottom.position.y, pong.position.y, 0.4, 0.4);

	player1.Update(deltaTime);
	player2.Update(deltaTime);
	pong.Update(deltaTime);
	


}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	player1.Render(&program);
	player2.Render(&program);
	pong.Render(&program);
	top.Render(&program);
	bottom.Render(&program);

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