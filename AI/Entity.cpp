#include "Entity.h"

Entity::Entity()
{
    entityType = PLATFORM;
    isStatic = true;
    isActive = true;
    position = glm::vec3(0);
    speed = 0;
    width = 1;
    height = 1;
}

bool Entity::CheckCollision(Entity& other)
{
    if (isStatic == true) return false;
    if (isActive == false || other.isActive == false) return false;
    
    float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
    float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
		if (entityType == PLAYER && other.entityType == ENEMY)
		{
			isActive = false;
		}
		if (entityType == SWORD && other.entityType == ENEMY)
		{
			isActive = false;
			other.isActive = false;
		}
		if (entityType == SWORD && other.entityType == PLAYER)
		{
			isActive = false;
			other.isActive = false;
		}
		if (entityType == SWORD && other.entityType == PLATFORM)
		{
			isActive = false;
		}
		if (entityType == ENEMY && other.entityType == PLAYER)
		{
			other.isActive = false;
		}
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {   
        if (CheckCollision(objects[i]))
        {
            float ydist = fabs(position.y - objects[i].position.y);
            float penetrationY = fabs(ydist - (height / 2) - (objects[i].height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        if (CheckCollision(objects[i]))
        {
            float xdist = fabs(position.x - objects[i].position.x);
            float penetrationX = fabs(xdist - (width / 2) - (objects[i].width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}


void Entity::Jump()
{
	
}

void Entity::Attack(Entity* blade)
{
	bladeTime = 40000;
	blade->isActive = true;
	if (currentAnim == walkRight)
	{
		blade->position.x = position.x + 1.0f;
		blade->position.y = position.y;
		blade->velocity.x = 7.0f;
		
	}
	else if (currentAnim == walkLeft)
	{
		blade->position.x = position.x - 1.0f;
		blade->position.y = position.y;
		blade->velocity.x = -7.0f;
	}
	else if (currentAnim == walkUp)
	{
		blade->position.x = position.x;
		blade->position.y = position.y + 1.0f;
		blade->velocity.y = 7.0f;
	}
	else if (currentAnim == walkDown)
	{
		blade->position.x = position.x;
		blade->position.y = position.y - 1.0f;
		blade->velocity.y = -7.0f;
	}
	
}

void Entity::AIWalker(Entity& player)
{
	switch (aiState) {
	case WALKLEFT:
		if (position.x > -6.0f) {
			velocity.x = -1.5f;
		}
		else
		{
			aiState = WALKRIGHT;
		}
		break;
	case WALKRIGHT:
		if (position.x < 6.0f)
		{
			velocity.x = 1.5f;
		}
		else {
			aiState = WALKLEFT;
		}
		break;
	}
}

void Entity::AIShooter(Entity& player, Entity& blade)
{
	switch (aiState) {
	case IDLE:
		if (glm::distance(position, player.position) < 3.0f) {
			aiState = SHOOTING;
		}
		break;
	case SHOOTING:
		if (abs(player.position.x - position.x) < 1.0f)
		{
			
			blade.isActive = true;
			if (player.position.y > position.y)
			{
				blade.position.x = position.x;
				blade.position.y = position.y + 1.0f;
				blade.velocity.y = 7.0f;
			}
			else
			{
				blade.position.x = position.x;
				blade.position.y = position.y - 1.0f;
				blade.velocity.y = -7.0f;
			}
		}
		if (abs(player.position.y - position.y) < 1.0f)
		{
			blade.isActive = true;
			if (player.position.x > position.x)
			{
				blade.position.x = position.x + 1.0f;
				blade.position.y = position.y;
				blade.velocity.x = 7.0f;
			}
			else
			{
				blade.position.x = position.x - 1.0f;
				blade.position.y = position.y;
				blade.velocity.x = -7.0f;
			}
		}
		break;
	}
}

void Entity::AIChaser(Entity& player)
{
	switch (aiState) {
	case IDLE:
		if (glm::distance(position, player.position) < 3.0f) {
			aiState = WALKING;
		}
		break;
	case WALKING:
		if (player.position.x > position.x)
		{
			velocity.x = 1.0f;
		}
		else if(player.position.x < position.x)
		{
			velocity.x = -1.0f;
		}

		if (player.position.y > position.y)
		{
			velocity.y = 1.0f;
		}
		else if (player.position.y < position.y)
		{
			velocity.y = -1.0f;
		}
		break;
	}
}

void Entity::AI(Entity& player, Entity& blade)
{
	switch (aiType)
	{
	case WALKER:
		AIWalker(player);
		break;
	case SHOOTER:
		AIShooter(player, blade);
		break;
	case CHASER:
		AIChaser(player);
		break;
	}
	
}

void Entity::Update(float deltaTime, Entity& player, Entity* blade, int bladeCount, Entity *objects, int objectCount, Entity *enemies, int enemyCount)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
	velocity += acceleration * deltaTime;
	/*while (bladeTime > 0)
	{
		bladeTime += -1 * deltaTime;
	}
	if (bladeTime == 0)
	{
		blade.isActive = false;
	}*/
	if (velocity.x == 0 && velocity.y == 0) return;

	if (velocity.x > 0)
	{
		currentAnim = walkRight;
	}
	if (velocity.x < 0)
	{
		currentAnim = walkLeft;
	}
	if (velocity.y > 0)
	{
		currentAnim = walkUp;
	}
	if (velocity.y < 0)
	{
		currentAnim = walkDown;
	}
	animTime += deltaTime;

	if (animTime >= 0.25f)
	{
		animTime = 0;
		animIndex++;
		if (animIndex >= animFrames)
		{
			animIndex = 0;
		}
	}

	if (entityType == PLAYER)
	{
		//CheckCollision(enemies[0]);
		//CheckCollision(enemies[1]);
		CheckCollisionsX(enemies, enemyCount);
		CheckCollisionsY(enemies, enemyCount);
		//CheckCollisionsX(&blade, 1);
		//CheckCollisionsY(&blade, 1);
	}
	if (entityType == ENEMY)
	{
		AI(player, blade[bladeCount - 1]);
		CheckCollisionsX(&player, 1);
		CheckCollisionsY(&player, 1);
		/*CheckCollisionsX(&blade, 1);
		CheckCollisionsY(&blade, 1);*/
	}
	if (entityType == SWORD)
	{
		CheckCollisionsX(enemies, enemyCount);
		CheckCollisionsY(enemies, enemyCount);
		CheckCollisionsX(&player, 1);
		CheckCollisionsY(&player, 1);

	}
	position.y += velocity.y * deltaTime;        // Move on Y
	CheckCollisionsY(objects, objectCount);    // Fix if needed

	position.x += velocity.x * deltaTime;        // Move on X
	CheckCollisionsX(objects, objectCount);    // Fix if needed

}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, int index)
{
	float u = (float)(index % cols) / (float)cols;
	float v = (float)(index / cols) / (float)rows;

	float width = 1.0f / (float)cols;
	float height = 1.0f / (float)rows;

	float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
		u, v + height, u + width, v, u, v };

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void Entity::Render(ShaderProgram *program) {
	if (isActive == true)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		program->SetModelMatrix(modelMatrix);

		if (entityType == PLAYER)
		{

			DrawSpriteFromTextureAtlas(program, currentAnim[animIndex]);

		}
		else
		{

			float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
			float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

			glBindTexture(GL_TEXTURE_2D, textureID);

			glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
			glEnableVertexAttribArray(program->positionAttribute);

			glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
			glEnableVertexAttribArray(program->texCoordAttribute);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(program->positionAttribute);
			glDisableVertexAttribArray(program->texCoordAttribute);
		}
	}
}

