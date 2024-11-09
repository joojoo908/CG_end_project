#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "glew32.lib")

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

#include<iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<math.h> //sqrt

#include <stdlib.h>
#include <stdio.h>

#include"shader.h"

#define ABS(X) ((X) < 0 ? -(X) : (X))
#define PI 3.141592

struct Color {
	float r, g, b;
};
struct Vertex {
	float x, y, z;
};
struct Face {
	unsigned int v1, v2, v3;
};
struct Model {
	std::vector<Vertex> vertices;
	std::vector<Color> colors;
	std::vector<Face> faces;
};

struct trans {
	glm::mat4 scale;
	glm::mat4 move;
	glm::mat4 spin;
};

void InitBuffer(const Model& model);
bool loadOBJ(const std::string& filename, Model& model);
GLvoid drawScene();
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);
GLvoid Reshape(int w, int h);

void clear_mat();
void move(glm::mat4& trans, Vertex move);
void spin(glm::mat4& trans, Vertex spin);
void vector_spin(std::vector<Model>& m, int num, Vertex mid, Vertex spin);
void mid_spin(glm::mat4& trans, Vertex mid, Vertex spin);
void scale(glm::mat4& trans);
void scale_v(glm::mat4& trans, Vertex scale);

Vertex mousevec(int x, int y);
float len_notsqrt(Vertex v1, Vertex v2);
float len(Vertex v1, Vertex v2);
Vertex subtract(Vertex a, Vertex b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}
//외적
Vertex crossProduct(const Vertex& v1, const Vertex& v2);
float sign(Vertex v1, Vertex v2, Vertex v3);
bool dot_in_tri(Vertex p, Vertex v1, Vertex v2, Vertex v3);

Vertex middle_Vertex(std::vector<Vertex> vertices) {
	Vertex m_v = { 0,0,0 };
	for (int i = 0; i < vertices.size(); i++) {
		m_v.x += vertices[i].x;
		m_v.y += vertices[i].y;
		m_v.z += vertices[i].z;
	}
	m_v.x /= vertices.size();
	m_v.y /= vertices.size();
	m_v.z /= vertices.size();
	return m_v;
}


void change_line(glm::mat4 t, Vertex& p1, Vertex& p2) {
	glm::mat4 inv = glm::inverse(t);
	glm::vec4 vertexPosition(p1.x, p1.y, p1.z, 1.0f);
	glm::vec4 vertexPosition2(p2.x, p2.y, p2.z, 1.0f);

	glm::vec4 transformedPosition = inv * vertexPosition;
	glm::vec4 transformedPosition2 = inv * vertexPosition2;

	p1.x = transformedPosition.x;
	p1.y = transformedPosition.y;
	p1.z = transformedPosition.z;

	p2.x = transformedPosition2.x;
	p2.y = transformedPosition2.y;
	p2.z = transformedPosition2.z;
}
void change_dot(glm::mat4 t, Vertex& p1) {
	glm::vec4 vertexPosition(p1.x, p1.y, p1.z, 1.0f);
	glm::vec4 transformedPosition = t * vertexPosition;
	p1.x = transformedPosition.x;
	p1.y = transformedPosition.y;
	p1.z = transformedPosition.z;
}

//--- 필요한 변수 선언-----------------------------------------------------------------------------------
GLint width = 1000, height = 800;

GLuint vao, vbo[2], ebo;
Vertex mouse;
//---------------------------------------------------------------------------------------------------
//Vertex camera_pos = { 0,0,0 };

Vertex vertex_spin(Vertex eye, Vertex& at, Vertex spin) {

	glm::mat4 Rx = glm::mat4(1.0f); //--- 이동 행렬 선언
	glm::mat4 Ry = glm::mat4(1.0f); //--- 회전 행렬 선언
	glm::mat4 Rz = glm::mat4(1.0f); //--- 회전 행렬 선언

	Rx = glm::rotate(Rx, glm::radians(spin.y), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(-spin.x), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(spin.z), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(-eye.x, -eye.y, -eye.z));
	glm::mat4 inv = glm::inverse(transformMatrix);

	glm::vec4 vertexPosition(at.x, at.y, at.z, 1.0f);
	glm::vec4 transformedPosition = inv * Rx * Ry * Rz * transformMatrix * vertexPosition;

	// 변환된 위치를 다시 model에 저장
	at.x = transformedPosition.x;
	at.y = transformedPosition.y;
	at.z = transformedPosition.z;

	return at;

}
class Camera {
private:
	Vertex eye = { 0,0,3 };
	Vertex at = { 0,0,0 };
	Vertex up = { 0,1,0 };

public:
	void print_c() {

	}
	void move_front() {
		float speed = 1;
		Vertex m_v = subtract(eye, at);
		m_v = { m_v.x / len(m_v,{}),m_v.y / len(m_v,{}) ,m_v.z / len(m_v,{}) };
		eye.x -= speed * m_v.x;
		eye.z -= speed * m_v.z;
		at.x -= speed * m_v.x;
		at.z -= speed * m_v.z;
	}
	void moving(float fb, float lr) {
		Vertex m_v = subtract(eye, at);
		m_v = { m_v.x / len(m_v,{}),m_v.y / len(m_v,{}) ,m_v.z / len(m_v,{}) };

		eye.x -= fb * m_v.x;
		eye.z -= fb * m_v.z;
		at.x -= fb * m_v.x;
		at.z -= fb * m_v.z;

		eye.x -= lr * -m_v.z;
		eye.z -= lr * m_v.x;
		at.x -= lr * -m_v.z;
		at.z -= lr * m_v.x;
	}
	void up_down(float ud) {

	}
	void spin(float x, float y, float z) {
		Vertex rotate = { x,y,z };
		vertex_spin(eye, at, rotate);
	}
	void view() {
		glm::vec3 Pos = glm::vec3(eye.x, eye.y, eye.z); //--- 카메라 위치
		//Vertex n = vertex_spin(eye, at, look);
		glm::vec3 At = glm::vec3(at.x, at.y, at.z); //--- 카메라 바라보는 방향
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(Pos, At, Up);

		unsigned int viewLocation = glGetUniformLocation(Shader::return_id(), "view"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.01f, 3000.0f); //--- 투영 공간 설정: fovy, aspect, near, far
		projection = glm::translate(projection, glm::vec3(0, 0, 0));
		unsigned int projectionLocation = glGetUniformLocation(Shader::return_id(), "projection"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};


Color RED = { 1, 0, 0 };
Color GREEN = { 0, 1, 0 };
Color BLUE = { 0, 0, 1 };

Model mid_line = { {{-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1} }, {{RED},{RED},{GREEN},{GREEN},{BLUE},{BLUE} } };
Model slice;

Model model;
Model dia;


std::vector<Color> colors = {
	{1.0, 0.0, 0.0}, // 빨간색
	{0.0, 1.0, 0.0}, // 초록색
	{0.0, 0.0, 1.0}, // 파란색
	{1.0, 1.0, 0.0}, // 노란색
	{0.0, 1.0, 1.0}, // 시안
	{1.0, 0.0, 1.0}, // 마젠타
	{0.0, 0.0, 0.0}, // 검은색
	{0.5, 0.5, 0.5},  // 흰색
};
bool cmdc = 0;
bool cmdh = 1;

Camera camera;

//Vertex spin_model;
Vertex spin_model2;
Vertex move_model;


Vertex shape_move2 = { 0.5,0,0 };

Vertex spin_xy = { 0,0,0 };
Vertex spin_y = { 0,1,0 };
Vertex spin_xy2 = { 0,0,0 };

int Timerspeed = 10;

int ani = 0;
int ani_cnt = 0;


//--------------------- 메인 함수----------------------------------------------------------------------------
int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1"); //--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	Shader::Shader();

	loadOBJ("cube.obj", model);
	loadOBJ("dia.obj", dia);

	for (int i = 0; i < model.vertices.size(); i++) {
		model.vertices[i].x -= 0.5;
		model.vertices[i].y -= 0.5;
		model.vertices[i].z -= 0.5;
	}
	for (int i = 0; i < dia.vertices.size(); i++) {
		dia.vertices[i].x -= 0.5;
		dia.vertices[i].y -= 0.5;
		dia.vertices[i].z -= 0.5;
	}

	//shaderProgramID = Shader::make_shaderProgram();

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glutTimerFunc(100, TimerFunction, 1);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (cmdh) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	//camara_test();
	camera.view();


	InitBuffer(mid_line);
	glPointSize(5.0);
	glDrawArrays(GL_LINES, 0, 6);

	clear_mat();

	glm::mat4 trans1(1.0f);
	
	scale(trans1);

	Model cpy;
	if (!cmdc) {
		cpy = model;
	}
	else {
		cpy = dia;
	}
	unsigned int transformLocation3 = glGetUniformLocation(Shader::return_id(), "transform");
	glUniformMatrix4fv(transformLocation3, 1, GL_FALSE, glm::value_ptr(trans1));
	InitBuffer(cpy);
	glDrawElements(GL_TRIANGLES, cpy.faces.size() * 3, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINE_STRIP, cpy.faces.size() * 3, GL_UNSIGNED_INT, 0);

	glutSwapBuffers(); // 화면에 출력하기
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		camera.moving(0.5, 0);
		break;
	case 's':
		camera.moving(-0.5, 0);
		break;
	case 'a':
		camera.moving(0, -0.5);
		break;
	case 'd':
		camera.moving(0, 0.5);
		break;
	case 'c':
		cmdc = !cmdc;
		break;
	case '1':
		camera.move_front();
		break;
	case 'h':
		cmdh = !cmdh;
		break;
	
	}
}
void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		//camera.spin(0.1, 0, 0);
		break;
	case GLUT_KEY_LEFT:
		//camera.spin(-0.1, 0, 0);
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse = mousevec(x, y);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		camera.print_c();
	}
	std::cout << "x = " << mouse.x << " y = " << mouse.y << std::endl;
}
void Motion(int x, int y)
{
	float sens = 30;
	Vertex mouse_motion = mousevec(x, y);
	camera.spin((mouse_motion.x - mouse.x) * sens, (mouse_motion.y - mouse.y) * sens, 0);
	mouse = mouse_motion;
}

void TimerFunction(int value)
{


	glutPostRedisplay(); // 화면 재 출력

	glutTimerFunc(Timerspeed, TimerFunction, 1); // 타이머함수 재 설정

}


//-------------------------------------------------------------------------

void clear_mat() {
	glm::mat4 transformMatrix(1.0f);
	unsigned int transformLocation = glGetUniformLocation(Shader::return_id(), "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
}
void move(glm::mat4& trans, Vertex move) {
	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(move.x, move.y, move.z));

	trans = trans * transformMatrix;

}
void spin(glm::mat4& trans, Vertex spin) {

	glm::mat4 Rx = glm::mat4(1.0f); //--- 이동 행렬 선언
	glm::mat4 Ry = glm::mat4(1.0f); //--- 회전 행렬 선언
	glm::mat4 Rz = glm::mat4(1.0f); //--- 회전 행렬 선언

	Rx = glm::rotate(Rx, glm::radians(spin.x), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(spin.y), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(spin.z), glm::vec3(0.0, 0.0, 1.0));

	trans *= Rx;
	trans *= Ry;
	trans *= Rz;

}
void mid_spin(glm::mat4& trans, Vertex mid, Vertex spin) {
	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(-mid.x, -mid.y, -mid.z));
	glm::mat4 inv = glm::inverse(transformMatrix);

	glm::mat4 Rx = glm::mat4(1.0f); //--- 이동 행렬 선언
	glm::mat4 Ry = glm::mat4(1.0f); //--- 회전 행렬 선언
	glm::mat4 Rz = glm::mat4(1.0f); //--- 회전 행렬 선언

	Rx = glm::rotate(Rx, glm::radians(spin.x), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(spin.y), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(spin.z), glm::vec3(0.0, 1.0, 0.0));


	trans = trans * inv * Rx * Ry * Rz * transformMatrix;
}
void vector_spin(std::vector<Model>& m, int num, Vertex mid, Vertex spin) {
	glm::mat4 Rx = glm::mat4(1.0f); //--- 이동 행렬 선언
	glm::mat4 Ry = glm::mat4(1.0f); //--- 회전 행렬 선언
	glm::mat4 Rz = glm::mat4(1.0f); //--- 회전 행렬 선언

	Rx = glm::rotate(Rx, glm::radians(spin.x), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(spin.y), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(spin.z), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(-mid.x, -mid.y, -mid.z));
	glm::mat4 inv = glm::inverse(transformMatrix);

	for (size_t i = 0; i < m[num].vertices.size(); i++) {
		glm::vec4 vertexPosition(m[num].vertices[i].x, m[num].vertices[i].y, m[num].vertices[i].z, 1.0f);
		glm::vec4 transformedPosition = inv * Rx * Ry * Rz * transformMatrix * vertexPosition;

		// 변환된 위치를 다시 model에 저장
		m[num].vertices[i].x = transformedPosition.x;
		m[num].vertices[i].y = transformedPosition.y;
		m[num].vertices[i].z = transformedPosition.z;
		//std::cout << spin.x << std::endl;
	}
}
void scale(glm::mat4& trans) {
	glm::mat4 sc = glm::mat4(1.0f);
	sc = glm::scale(sc, glm::vec3(0.5, 0.5, 0.5));
	trans *= sc;
}
void scale_v(glm::mat4& trans, Vertex scale) {
	glm::mat4 sc = glm::mat4(1.0f);
	sc = glm::scale(sc, glm::vec3(scale.x, scale.y, scale.z));
	trans *= sc;
}

//마우스 좌표변환
Vertex mousevec(int x, int y) {
	Vertex alpa = { (((float)x - (width / 2)) / (width / 2)) ,  (height / 2 - (float)y) / (height / 2)  , 0 };
	return alpa;
}
float len_notsqrt(Vertex v1, Vertex v2) {
	return (v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z);
}
float len(Vertex v1, Vertex v2) {
	return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
}
Vertex crossProduct(const Vertex& v1, const Vertex& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}
//삼각형 내부의 점 체크
float sign(Vertex v1, Vertex v2, Vertex v3) {
	return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.y) * (v1.y - v3.y);
}
bool dot_in_tri(Vertex p, Vertex v1, Vertex v2, Vertex v3) {
	float d1, d2, d3;
	bool neg, pos;

	d1 = sign(p, v1, v2);
	d2 = sign(p, v2, v3);
	d3 = sign(p, v3, v1);

	neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(neg && pos);
}


//-------------------------------------------------------------------------------
void InitBuffer(const Model& model) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO 생성 및 정점 데이터 전송
	glGenBuffers(1, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// 색상 VBO 생성 및 색상 데이터 전송
	glGenBuffers(1, &vbo[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// EBO 생성 및 데이터 전송
	if (!model.faces.empty()) {
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		std::vector<unsigned int> indices;
		for (const auto& face : model.faces) {
			indices.push_back(face.v1);
			indices.push_back(face.v2);
			indices.push_back(face.v3);
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	}

	//glBindVertexArray(0);
	glBindVertexArray(vao);
}
bool loadOBJ(const std::string& filename, Model& model) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "파일을 열 수 없습니다: " << filename << std::endl;
		return false;
	}

	int i = 0;
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			Vertex vertex;
			iss >> vertex.x >> vertex.y >> vertex.z;
			model.vertices.push_back(vertex);
			model.colors.push_back(colors[i]);
			i++;
		}
		else if (prefix == "vn") {

		}
		else if (prefix == "f") {
			Face face;
			unsigned int v[3], n[3];
			char slash;

			for (int i = 0; i < 3; ++i) {
				if (iss >> v[i]) {
					// v/n 형식일 때 (v/n이 있을 경우)
					if (iss.peek() == '/') {
						iss >> slash; // 첫 번째 '/' 읽기
						if (iss.peek() == '/') {
							iss >> slash; // 두 번째 '/' 읽기 (법선 인덱스가 없을 때)
							iss >> n[i];
						}
					}
				}
			}
			face.v1 = v[0] - 1;
			face.v2 = v[1] - 1;
			face.v3 = v[2] - 1;
			model.faces.push_back(face);
		}
	}

	return true;
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
