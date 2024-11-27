#pragma once

#include "gl/glm/glm.hpp"
class Model;
class Terrain;

class Player
{
public:
	Player(Model* model);
	void HandleInput(unsigned char keys,bool updown ,float deltaTime);
	bool Move(float deltaTime);
	float GetRotY();

	Model* GetModel() { return model; }

private:
	void Jump();

	Model* model;

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed;
	float currTurnSpeed;
	float upwardSpeed;
	float groundHeight;

	bool isJumping;
};
