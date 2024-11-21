#include "FreeCamera.h"

#include <gl/glm/glm.hpp>
#include <gl/glm\gtc\matrix_transform.hpp>
#include <gl/glm\gtc\type_ptr.hpp>

//#include "Window.h"

FreeCamera::FreeCamera(glm::vec3 startPosition, GLfloat moveSpeed, GLfloat turnSpeed,
	GLfloat startPitch, GLfloat startYaw) : CameraBase(startPosition, startPitch, startYaw)
{
	this->moveSpeed = moveSpeed;
	this->turnSpeed = turnSpeed;

	canMove = false;
}


void FreeCamera::KeyControl(unsigned char keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

    if (keys == 'w') {
        position += front * velocity; // W키를 눌렀을 때 전방으로 이동
    }
    if (keys == 's') {
        position -= front * velocity; // S키를 눌렀을 때 후방으로 이동
    }
    if (keys == 'a') {
        position -= right * velocity; // A키를 눌렀을 때 좌측으로 이동
    }
    if (keys == 'd') {
        position += right * velocity; // D키를 눌렀을 때 우측으로 이동
    }
    if (keys == 'e') {
        position += up * velocity; // 스페이스바를 눌렀을 때 위로 이동
    }
    if (keys == 'q') { // Shift 키
        position -= up * velocity; // Shift 키를 눌렀을 때 아래로 이동
    }
}

void FreeCamera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;
}

void FreeCamera::ScrollControl(GLfloat scrollY)
{
	moveSpeed += scrollY * 5.f;
	moveSpeed = std::max(0.1f, moveSpeed);
}

FreeCamera::~FreeCamera()
{
}
