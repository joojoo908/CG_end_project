#pragma once
#include <GL/freeglut.h>
#include <gl/GLM/glm.hpp>
#include <gl/GLM/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);


	void keyControl(unsigned char key, int x, int y); // 키보드 입력 처리
	void specialKeyControl(int key, int x, int y); // 특수키 입력 처리
	void mouseControl(int x, int y); // 마우스 입력 처리
	void mouseClick(int button, int state, int x, int y);
	glm::mat4 calculateViewMatrix();

private:
	glm::vec3 position; // 카메라 위치
	glm::vec3 front; // 카메라 바라보는 방향
	glm::vec3 up; // 카메라 상단
	glm::vec3 right; // 카메라 오른쪽
	glm::vec3 worldUp; // 월드에서의 상 방향

	GLfloat yaw; // 수평 회전
	GLfloat pitch; // 수직 회전

	GLfloat moveSpeed; // 이동 속도
	GLfloat turnSpeed; // 회전 속도

	// 카메라 좌표계 업데이트
	void update();
	bool isMousePressed;

	// 마우스 위치 저장
	GLfloat lastX, lastY;
	GLfloat xChange, yChange;
};