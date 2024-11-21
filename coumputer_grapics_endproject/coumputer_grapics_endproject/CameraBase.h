#pragma once

#include <GL\glew.h>

#include <gl/glm\glm.hpp>
#include <gl/glm\gtc\matrix_transform.hpp>

class CameraBase
{
public:
	CameraBase(glm::vec3 startPosition, GLfloat startPitch = 0.f, GLfloat startYaw = -90.f);

	virtual void Update();
	virtual void KeyControl(unsigned char keys, GLfloat deltaTime) = 0;
	virtual void MouseControl(GLfloat xChange, GLfloat yChange) = 0;
	virtual void ScrollControl(GLfloat scrollY) = 0;

	bool CanMove() { return canMove; }
	void SetCanMove(bool flag) { canMove = flag; }

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix(GLfloat width, GLfloat height);
	glm::vec3 GetPosition();


protected:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat nearClippingPlane;
	GLfloat farClippingPlane;

	bool canMove;
};

