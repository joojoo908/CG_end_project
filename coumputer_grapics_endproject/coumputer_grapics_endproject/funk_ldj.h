#pragma once
#pragma comment(lib, "glew32.lib")

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>


#include <vector>
#include"shader.h"

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

class view_size {
private:
	static GLint wid;
	static GLint heig;
public:
	void setting(GLint w, GLint h);
	static GLint &width();
	static GLint &height();
};

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
Vertex subtract(Vertex a, Vertex b);
//¿ÜÀû
Vertex crossProduct(const Vertex& v1, const Vertex& v2);
float sign(Vertex v1, Vertex v2, Vertex v3);
bool dot_in_tri(Vertex p, Vertex v1, Vertex v2, Vertex v3);

Vertex middle_Vertex(std::vector<Vertex> vertices);


void change_line(glm::mat4 t, Vertex& p1, Vertex& p2);
void change_dot(glm::mat4 t, Vertex& p1);
