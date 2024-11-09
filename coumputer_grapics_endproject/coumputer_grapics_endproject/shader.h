#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<iostream>

class Shader {
private:
	//GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
	GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
	GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
	GLint result;
	GLchar errorLog[512];
	static GLuint shaderID;
public:
	Shader();
	static GLuint& return_id();

	char* filetobuf(const char* file);
	void make_vertexShaders();
	void make_fragmentShaders();
	GLuint make_shaderProgram();
};



