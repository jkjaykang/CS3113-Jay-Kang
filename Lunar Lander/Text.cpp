#include "Text.h"

Text::Text()
{
	isStatic = true;
	isActive = true;
	position = glm::vec3(0);
}

void Text::DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
	float width = 1.0f / 16.0f;
	float height = 1.0f / 16.0f;

	//std::vector<float> vertices;
	//std::vector<float> texCoords;

	for (int i = 0; i < text.size(); i++)
	{
		int index = (int)text[i];

		float u = (float)(index % 16) / 16.0f;
		float v = (float)(index / 16) / 16.0f;

		texCoords.insert(texCoords.end(), { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v });

		float offset = (size + spacing) * i;

		vertices.insert(vertices.end(), { offset + (-0.5f * size), (-0.5f * size),
			offset + (0.5f * size), (-0.5f * size), offset + (0.5f * size), (0.5f * size),
			offset + (-0.5f * size), (-0.5f * size), offset + (0.5f * size), (0.5f * size),
			offset + (-0.5f * size), (0.5f * size) });
	}
}

void Text::Update(float deltaTime, Text* objects, int objectCount)
{
}


void Text::Render(ShaderProgram* program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);
	
	glBindTexture(GL_TEXTURE_2D, fontTextureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2.0f);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}
