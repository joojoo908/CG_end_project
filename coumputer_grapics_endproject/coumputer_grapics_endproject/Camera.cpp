#include "Camera.h"

// 기본 생성자
Camera::Camera()
    : position(glm::vec3(0.0f, 0.0f, 3.0f)), front(glm::vec3(0.0f, 0.0f, -1000.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(-90.0f), pitch(0.0f),
    moveSpeed(2.5f), turnSpeed(0.1f), lastX(0.0f), lastY(0.0f), xChange(0.0f), yChange(0.0f)
{
    update();
}

// 생성자
Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
    : position(startPosition), up(startUp), yaw(startYaw), pitch(startPitch),
    moveSpeed(startMoveSpeed), turnSpeed(startTurnSpeed), lastX(0.0f), lastY(0.0f), xChange(0.0f), yChange(0.0f)
{
    front = glm::vec3(0.0f, 0.0f, -1.0f); // 기본적으로 카메라는 -z 방향을 봄
    right = glm::normalize(glm::cross(up, front)); // 카메라의 오른쪽 벡터 계산
    worldUp = startUp; // 월드에서의 상 방향
    update();
}

// 키보드 입력 처리
void Camera::keyControl(unsigned char key, int x, int y)
{
    GLfloat velocity = moveSpeed * 0.1f; // 이동 속도 조정 (프레임 기준)

    if (key == 27) { // ESC 키를 눌렀을 때 창을 닫도록 설정
        exit(0);
    }

    if (key == 'w') {
        position += front * velocity; // W키를 눌렀을 때 전방으로 이동
    }
    if (key == 's') {
        position -= front * velocity; // S키를 눌렀을 때 후방으로 이동
    }
    if (key == 'a') {
        position -= right * velocity; // A키를 눌렀을 때 좌측으로 이동
    }
    if (key == 'd') {
        position += right * velocity; // D키를 눌렀을 때 우측으로 이동
    }
    if (key == ' ') {
        position += up * velocity; // 스페이스바를 눌렀을 때 위로 이동
    }
    if (key == 0x10) { // Shift 키
        position -= up * velocity; // Shift 키를 눌렀을 때 아래로 이동
    }

    update();
}

// 특수 키 입력 처리 (예: 방향키)
void Camera::specialKeyControl(int key, int x, int y)
{
    // 특수키 처리 (예: 방향키)
    if (key == GLUT_KEY_F1) {
        // F1 키가 눌렸을 때 처리할 로직
    }
}

void Camera::mouseClick(int button, int state, int x, int y)
{
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isMousePressed = false;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isMousePressed = true;
        lastX = x;
        lastY = y;
    }
}

// 마우스 입력 처리
void Camera::mouseControl(int x, int y)
{
    if (isMousePressed) {

        GLfloat xOffset = x - lastX;
        GLfloat yOffset = lastY - y; // 마우스 Y축은 반대로 증가 (화면 좌표계와 맞추기 위해)

        lastX = x;
        lastY = y;

        xChange = xOffset;
        yChange = yOffset;

        yaw += xChange * turnSpeed; // 마우스 X 이동에 따라 Yaw 회전
        pitch -= yChange * turnSpeed; // 마우스 Y 이동에 따라 Pitch 회전

        // 피치 제한 (고개를 너무 올리거나 내리지 않도록)
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

    }

    update();
}

// 카메라 좌표계를 업데이트
void Camera::update()
{
    // 카메라의 바라보는 방향 벡터를 계산
    glm::vec3 frontTemp;
    frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontTemp.y = sin(glm::radians(pitch));
    frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontTemp);

    // 오른쪽 벡터 계산
    right = glm::normalize(glm::cross(front, worldUp));

    // 상 방향 벡터 계산
    up = glm::normalize(glm::cross(right, front));
}

// 카메라의 뷰 행렬 계산
glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position, position + front, up); // 카메라의 위치, 바라보는 방향, 상 방향
}