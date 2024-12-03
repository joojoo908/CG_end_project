#include "Player.h"

#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "ShaderHandle.h"

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Player::Player(Model* model, Animator* animator) : MOVE_SPEED(10.f), TURN_SPEED(200.f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	this->animator = animator;
	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Knight/idle.gltf", model);
	danceAnim = new Animation("Knight/dance.gltf", model);
	runAnim = new Animation("Knight/run.gltf", model);

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

void Player::update(float deltaTime) {
	
	if (Move(deltaTime))
	{
		if (animator->GetCurrAnimation() != runAnim)
			animator->PlayAnimation(runAnim);
	}
	else
	{
		if (animator->GetCurrAnimation() != idleAnim)
			animator->PlayAnimation(idleAnim);
	}
	//애니메이션 업데이트
	animator->UpdateAnimation(deltaTime);
	
}

void Player::draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount) {
	glm::mat4 viewMat = currCamera->GetViewMatrix();
	glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
	glm::vec3 camPos = currCamera->GetPosition();

	shaderList[0]->UseShader();

	GetShaderHandles();

	glm::mat4 modelMat = model->GetModelMat();
	glm::mat4 PVM = projMat * viewMat * modelMat;
	glm::mat3 normalMat = GetNormalMat(modelMat);

	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

	shaderList[0]->UseEyePos(camPos);
	shaderList[0]->UseDirectionalLight(directionalLight);
	shaderList[0]->UsePointLights(pointLights, pointLightCount);

	shaderList[0]->UseMaterial(model->GetMaterial());

	const auto& transforms = animator->GetFinalBoneMatrices();
	shaderList[0]->UseFinalBoneMatrices(transforms);

	glUniform1i(loc_diffuseSampler, 0);
	glUniform1i(loc_normalSampler, 1);

	model->RenderModel();

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}