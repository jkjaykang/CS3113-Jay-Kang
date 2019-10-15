#pragma once
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

class Text {
public:

	bool isStatic;
	bool isActive;

	glm::vec3 position;

	std::vector<float> vertices;
	std::vector<float> texCoords;

	GLuint fontTextureID;

	Text();

	void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);
	void Update(float deltaTime, Text* objects, int objectCount);
	void Render(ShaderProgram* program);

};



