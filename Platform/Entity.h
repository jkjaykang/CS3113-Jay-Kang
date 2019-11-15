#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Map.h"

enum  EntityType { PLAYER, PLATFORM, COIN };


class Entity {
public:
    
    EntityType entityType;
    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    
    float speed;
    
    GLuint textureID;

	int cols;
	int rows;
	int* walkRight;
	int* walkLeft;
	int* jumpUp;
	int* currentAnim;
	int animFrames;
	int animIndex;
	float animTime;

    Entity();
    
    bool CheckCollision(Entity other);

	void CheckCollisionsX(Map* map);
	void CheckCollisionsY(Map* map);

    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);

	void DrawSpriteFromTextureAtlas(ShaderProgram* program, int index);

    void Update(float deltaTime, Entity *objects, int objectCount, Map *map);
    void Render(ShaderProgram *program);
    
    void Jump();
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
};



