#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum  EntityType { PLAYER, PLATFORM, COIN, ENEMY, SWORD, BULLET };

enum AIState{IDLE, WALKING, WALKLEFT, WALKRIGHT, SHOOTING};

enum AIType {WALKER, SHOOTER, CHASER};

class Entity {
public:
    
    EntityType entityType;
	AIState aiState;
	AIType aiType;

    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
	float width = 1.0f;
    float height = 1.0f;
    
    float speed;
	float bladeTime;

    GLuint textureID;
	int cols;
	int rows;
	int* walkRight;
	int* walkLeft;
	int* walkUp;
	int* walkDown;
	int* currentAnim;
	int animFrames;
	int animIndex;
	float animTime;
    
    Entity();
    
	void AI(Entity& player, Entity& blade);
	void AIWalker(Entity& player);
	void AIShooter(Entity& player, Entity& blade);
	void AIChaser(Entity& player);


    bool CheckCollision(Entity& other);
    
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    
	void Update(float deltaTime, Entity& player, Entity* blade, int bladeCount, Entity* objects, int objectCount, Entity* enemies, int enemyCount);
    void Render(ShaderProgram *program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, int index);
    
    void Jump();
	void Attack(Entity* blade);
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
	bool movingTop;
	bool movingBottom;
	bool movingLeft;
	bool movingRight;

};



