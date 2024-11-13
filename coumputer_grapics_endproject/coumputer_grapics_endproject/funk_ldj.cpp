#include"funk_ldj.h"

GLint view_size::wid = 1000;
GLint view_size::heig = 800;

GLint& view_size::width() {
	return wid;
}
GLint& view_size::height() {
	return heig;
}

void view_size::setting(GLint w, GLint h) {
	wid = w;
	heig = h;
}

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
	Vertex alpa = { (((float)x - (view_size::width() / 2)) / (view_size::width() / 2)) ,  (view_size::height() / 2 - (float)y) / (view_size::height() / 2)  , 0 };
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

Vertex subtract(Vertex a, Vertex b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
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