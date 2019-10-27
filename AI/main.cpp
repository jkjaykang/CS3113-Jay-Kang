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

#define PLATFORM_COUNT 86
#define ENEMY_COUNT 3
#define BLADE_COUNT 2
#define TEXT_COUNT 2

struct GameState {
	Entity player;
	Entity blade[BLADE_COUNT];
    Entity platforms[PLATFORM_COUNT];
	Entity enemies[ENEMY_COUNT];
	Text gameText[TEXT_COUNT];
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
    displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.width = 0.6f;
	state.player.height = 0.6;
    state.player.position = glm::vec3(1, 4, 0);
    state.player.acceleration = glm::vec3(0, 0, 0);
    state.player.textureID = LoadTexture("george_0.png");
	state.player.cols = 4;
	state.player.rows = 4;
	state.player.walkRight = new int[4]{ 3, 7, 11, 15 };
	state.player.walkLeft = new int[4]{ 1, 5, 9, 13 };
	state.player.walkUp = new int[4]{ 2, 6, 10, 14 };
	state.player.walkDown = new int[4]{ 4, 8, 12, 16 };
	state.player.currentAnim = state.player.walkRight;
	state.player.animFrames = 4;

	state.blade[0].entityType = SWORD;
	state.blade[0].width = 0.5f;
	state.blade[0].height = 0.5f;
	state.blade[0].isStatic = false;
	state.blade[0].isActive = false;
	state.blade[0].textureID = LoadTexture("fire.png");
	//state.blade.position = glm::vec3(0, 0, 0);

	state.blade[1].entityType = SWORD;
	state.blade[1].width = 0.5f;
	state.blade[1].height = 0.5f;
	state.blade[1].isStatic = false;
	state.blade[1].isActive = false;
	state.blade[1].textureID = LoadTexture("efire.png");
	//state.blade[1].velocity.x = 0.1f;


	GLuint walkerTextureID = LoadTexture("crab.png");
	GLuint chaserTextureID = LoadTexture("bluecrab.png");
	GLuint shooterTextureID = LoadTexture("armycrab.png");

	state.enemies[0].entityType = ENEMY;
	state.enemies[0].textureID = chaserTextureID;
	state.enemies[0].isStatic = false;
	state.enemies[0].height = 0.5;
	state.enemies[0].width = 0.5;
	state.enemies[0].position = glm::vec3(1, -4.5, 0);
	state.enemies[0].velocity.x = -1.0f;
	state.enemies[0].aiState = WALKING;
	state.enemies[0].aiType = CHASER;

	state.enemies[1].entityType = ENEMY;
	state.enemies[1].textureID = walkerTextureID;
	state.enemies[1].isStatic = false;
	state.enemies[1].height = 0.5;
	state.enemies[1].width = 0.5;
	state.enemies[1].position = glm::vec3(-1, 4, 0);
	state.enemies[1].velocity.x = -1.0f;
	state.enemies[1].aiState = WALKLEFT;
	state.enemies[1].aiType = WALKER;

	state.enemies[2].entityType = ENEMY;
	state.enemies[2].textureID = shooterTextureID;
	state.enemies[2].isStatic = false;
	state.enemies[2].height = 0.5;
	state.enemies[2].width = 0.5;
	state.enemies[2].position = glm::vec3(3, -4, 0);
	state.enemies[2].aiState = SHOOTING;
	state.enemies[2].aiType = SHOOTER;
	state.enemies[2].velocity.x = -0.0001f;

	GLuint wordTextureID = LoadTexture("font.png");
	state.gameText[0].fontTextureID = wordTextureID;
	state.gameText[0].position = glm::vec3(-4, 5.5, 0);
	state.gameText[1].fontTextureID = wordTextureID;
	state.gameText[1].position = glm::vec3(-2.75, 0, 0);


    GLuint dirtTextureID = LoadTexture("dirt.png");
    GLuint grassTextureID = LoadTexture("grass.png");
    
	for (int i = 0; i < 15; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-7.0f + i, -5.75f, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 15; i < 27; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(7.0f, -19.75f + i, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 27; i < 39; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-7.0f, -31.75f + i, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 39; i < 54; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-46.0f + i, 5.75f, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 54; i < 58; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-58.0f + i, 3, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 58; i < 62; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-57.0f + i, 3, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 62; i < 66; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-68.0f + i, 1, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 66; i < 70; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-67.0f + i, 1, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 70; i < 74; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-79.0f + i, -1, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 74; i < 78; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-78.0f + i, -1, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 78; i < 82; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-89.0f + i, -3, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
	for (int i = 82; i < 86; i++)
	{
		state.platforms[i].textureID = dirtTextureID;
		state.platforms[i].position = glm::vec3(-88.0f + i, -3, 0);
		state.platforms[i].width = 0.8;
		state.platforms[i].height = 0.8;
	}
    
    

    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-7.0f, 7.0f, -5.75f, 5.75f, -1.0f, 1.0f);

    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
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
                        state.player.Jump();
                        break;
					case SDLK_k:
						if (state.blade[0].isActive == false)
						{
							state.player.Attack(&state.blade[0]);
						}
						break;
                }
                break;
        }
    }
    
	state.player.velocity.x = 0;
	state.player.velocity.y = 0;
	

    // Check for pressed/held keys below
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
   
    if (keys[SDL_SCANCODE_A])
    {
        state.player.velocity.x = -3.0f;
		state.player.movingLeft = true;
		state.player.movingRight = false;
    }
    else if  (keys[SDL_SCANCODE_D])
    {
        state.player.velocity.x = 3.0f;
		state.player.movingRight = true;
		state.player.movingLeft = false;
    }
	else if (keys[SDL_SCANCODE_W])
	{
		state.player.velocity.y = 3.0f;
		state.player.movingTop = true;
		state.player.movingBottom = false;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		state.player.velocity.y = -3.0f;
		state.player.movingBottom = true;
		state.player.movingTop = false;
	}
	

	/*if (glm::length(state.player.velocity) > 3.0f)
	{
		state.player.velocity = glm::normalize(state.player.velocity);
	}*/
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
        state.player.Update(FIXED_TIMESTEP, state.player, state.blade, BLADE_COUNT, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
		state.blade[0].Update(FIXED_TIMESTEP, state.player, state.blade, BLADE_COUNT, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
		state.blade[1].Update(FIXED_TIMESTEP, state.player, state.blade, BLADE_COUNT, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);

		for (int i = 0; i < ENEMY_COUNT; i++)
		{
			state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.blade, BLADE_COUNT, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
		}
        deltaTime -= FIXED_TIMESTEP;

    }
	state.gameText[0].DrawText(&program, state.gameText[1].fontTextureID, "Press K to fire", 0.5, 0.1, state.gameText[1].position);
	if (state.player.isActive == false)
	{
		state.gameText[1].DrawText(&program, state.gameText[1].fontTextureID, "Game Over", 0.5, 0.15, state.gameText[1].position);
	}
	if (state.enemies[0].isActive == false && state.enemies[1].isActive == false && state.enemies[2].isActive == false)
	{
		state.gameText[1].DrawText(&program, state.gameText[1].fontTextureID, "Best Chef", 0.5, 0.15, state.gameText[1].position);

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

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		state.enemies[i].Render(&program);
	}

	for (int i = 0; i < BLADE_COUNT; i++)
	{
		state.blade[i].Render(&program);
	}
	state.gameText[0].Render(&program);
	state.gameText[1].Render(&program);

    
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
