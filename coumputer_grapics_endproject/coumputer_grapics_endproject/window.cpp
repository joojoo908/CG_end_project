
#include <fstream>
#include <sstream>
#include <string>
#include<math.h> //sqrt

#include <stdlib.h>
#include <stdio.h>

#include"shader.h"
#include"funk_ldj.h"

#define PI 3.141592

void InitBuffer(const Model& model);
bool loadOBJ(const std::string& filename, Model& model);

GLvoid drawScene();
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);
GLvoid Reshape(int w, int h);

//--- �ʿ��� ���� ����-----------------------------------------------------------------------------------
//GLint width = 1000, height = 800;


GLuint vao, vbo[2], ebo;
Vertex mouse;
//---------------------------------------------------------------------------------------------------
//Vertex camera_pos = { 0,0,0 };

Vertex vertex_spin(Vertex eye, Vertex& at, Vertex spin) {

	glm::mat4 Rx = glm::mat4(1.0f); //--- �̵� ��� ����
	glm::mat4 Ry = glm::mat4(1.0f); //--- ȸ�� ��� ����
	glm::mat4 Rz = glm::mat4(1.0f); //--- ȸ�� ��� ����

	Rx = glm::rotate(Rx, glm::radians(spin.x), glm::vec3(1.0, 0.0, 0.0));
	Ry = glm::rotate(Ry, glm::radians(spin.y), glm::vec3(0.0, 1.0, 0.0));
	Rz = glm::rotate(Rz, glm::radians(spin.z), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 transformMatrix(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(-eye.x, -eye.y, -eye.z));
	glm::mat4 inv = glm::inverse(transformMatrix);

	glm::vec4 vertexPosition(at.x, at.y, at.z, 1.0f);
	glm::vec4 transformedPosition = inv * Rx * Ry * Rz * transformMatrix * vertexPosition;

	// ��ȯ�� ��ġ�� �ٽ� model�� ����
	at.x = transformedPosition.x;
	at.y = transformedPosition.y;
	at.z = transformedPosition.z;

	return at;

}
class Camera {
private:
	Vertex eye = { 0,3,10 };
	Vertex at = { 0,0,0 };
public:
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
		eye.y += ud;
		at.y += ud;
	}
	void spin(float x, float y, float z) {
		Vertex rotatex = { 0,atan(x),0 };
		Vertex rotatey = { sin(atan2(eye.z - at.z,eye.x - at.x)) * -atan(y), 0 , -cos(atan2(eye.z - at.z,eye.x - at.x)) * -atan(y) };

		vertex_spin(eye, at, rotatex);
		vertex_spin(eye, at, rotatey);
	}
	void spin_eye(float x) {
		Vertex rotatex = { 0,atan(x),0 };
		vertex_spin(at, eye, rotatex);
	}
	void view() { //��������
		glm::vec3 Pos = glm::vec3(eye.x, eye.y, eye.z); //--- ī�޶� ��ġ
		//Vertex n = vertex_spin(eye, at, look);
		glm::vec3 At = glm::vec3(at.x, at.y, at.z); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(Pos, At, Up);

		unsigned int viewLocation = glGetUniformLocation(Shader::return_id(), "view"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)view_size::width() / (float)view_size::height(), 0.01f, 3000.0f); //--- ���� ���� ����: fovy, aspect, near, far
		projection = glm::translate(projection, glm::vec3(0, 0, 0));
		unsigned int projectionLocation = glGetUniformLocation(Shader::return_id(), "projection"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		//glViewport(400, 0, 400, 400);
	}
	void view2() { //��������
		glm::vec3 Pos = glm::vec3(eye.x, eye.y, eye.z); //--- ī�޶� ��ġ
		//Vertex n = vertex_spin(eye, at, look);
		glm::vec3 At = glm::vec3(at.x, at.y, at.z); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(Pos, At, Up);

		unsigned int viewLocation = glGetUniformLocation(Shader::return_id(), "view"); //--- ���� ��ȯ ����
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		glm::mat4 projection = glm::mat4(1.0f);
		float ort = 10;
		projection = glm::ortho(-ort, ort, -ort, ort, -ort, ort);

		unsigned int projectionLocation = glGetUniformLocation(Shader::return_id(), "projection"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
};
Color RED = { 1, 0, 0 };
Color GREEN = { 0, 1, 0 };
Color BLUE = { 0, 0, 1 };

Model mid_line = { {{-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1} }, {{RED},{RED},{GREEN},{GREEN},{BLUE},{BLUE} } };


Model model;
Model dia;


std::vector<Color> colors = {
	{1.0, 0.0, 0.0}, // ������
	{0.0, 1.0, 0.0}, // �ʷϻ�
	{0.0, 0.0, 1.0}, // �Ķ���
	{1.0, 1.0, 0.0}, // �����
	{0.0, 1.0, 1.0}, // �þ�
	{1.0, 0.0, 1.0}, // ����Ÿ
	{0.0, 0.0, 0.0}, // ������
	{0.5, 0.5, 0.5},  // ���
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


//--------------------- ���� �Լ�----------------------------------------------------------------------------
int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(view_size::width(), view_size::height());
	glutCreateWindow("Example1"); //--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
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

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glutTimerFunc(100, TimerFunction, 1);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
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

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
		
		break;
	case 'h':
		cmdh = !cmdh;
		break;
	
	}
}
void SpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		camera.up_down(0.5);
		break;
	case GLUT_KEY_DOWN:
		camera.up_down(-0.5);
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse = mousevec(x, y);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		
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


	glutPostRedisplay(); // ȭ�� �� ���

	glutTimerFunc(Timerspeed, TimerFunction, 1); // Ÿ�̸��Լ� �� ����

}

//-------------------------------------------------------------------------------
void InitBuffer(const Model& model) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO ���� �� ���� ������ ����
	glGenBuffers(1, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// ���� VBO ���� �� ���� ������ ����
	glGenBuffers(1, &vbo[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// EBO ���� �� ������ ����
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
		std::cerr << "������ �� �� �����ϴ�: " << filename << std::endl;
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
					// v/n ������ �� (v/n�� ���� ���)
					if (iss.peek() == '/') {
						iss >> slash; // ù ��° '/' �б�
						if (iss.peek() == '/') {
							iss >> slash; // �� ��° '/' �б� (���� �ε����� ���� ��)
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
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
