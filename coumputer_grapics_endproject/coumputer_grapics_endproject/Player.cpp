#include "Player.h"

#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>

#include "Model.h"
//#include "Terrain.h"

Player::Player(Model* model) : MOVE_SPEED(10.f), TURN_SPEED(200.f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	groundHeight = 10;
	upwardSpeed = 0;

	isJumping = true;
}

void Player::HandleInput(unsigned char keys,bool updown , float deltaTime)
{
	if (updown) {
		if (keys == 'w')
			currMoveSpeed = MOVE_SPEED;
		else if (keys == 's')
			currMoveSpeed = -MOVE_SPEED;
		else
			currMoveSpeed = 0;

		if (keys == 'a')
			currTurnSpeed = TURN_SPEED;
		else if (keys == 'd')
			currTurnSpeed = -TURN_SPEED;
		else
			currTurnSpeed = 0;
	}
	else {
		if (keys == 'w')
			currMoveSpeed = 0;
		else if (keys == 's')
			currMoveSpeed = 0;
		

		if (keys == 'a')
			currTurnSpeed = 0;
		else if (keys == 'd')
			currTurnSpeed = 0;
	}
	//점프 막음
	/*if (keys=='j')
		Jump();*/

}

bool Player::Move(float deltaTime)
{



	// 회전
	GLfloat* currRot = model->GetRotate();

	float rotation = currTurnSpeed * deltaTime;

	float newRotY = currRot[1] + rotation; // new rotY
	if (newRotY > 180)
		newRotY -= 360.f;
	if (newRotY < -180.f)
		newRotY += 360.f;

	glm::vec3 newRot(currRot[0], newRotY, currRot[2]);

	model->SetRotate(newRot);

	// 이동
	GLfloat* currPos = model->GetTranslate();
	float distance = currMoveSpeed * deltaTime;

	float dx = distance * sinf(glm::radians(newRotY));
	float dz = distance * cosf(glm::radians(newRotY));

	//중력제거
	//upwardSpeed -= GRAVITY * deltaTime;

	glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

	// 땅과의 충돌
	//groundHeight = terrain->GetHeight(currPos[0], currPos[2]);
	//if (newPos[1] <= groundHeight) // 땅에 닿았다면
	//{
	//	upwardSpeed = 0;
	//	newPos[1] = groundHeight;
	//	isJumping = false;
	//}

	model->SetTranslate(newPos);

	return currMoveSpeed != 0;
}

float Player::GetRotY()
{
	return model->GetRotate()[1];
}

void Player::Jump()
{
	if (isJumping)
		return;

	upwardSpeed = JUMP_POWER;
	isJumping = true;
}