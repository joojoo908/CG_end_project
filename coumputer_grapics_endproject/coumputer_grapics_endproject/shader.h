#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include<iostream>

class Shader {
private:
	//GLuint shaderProgramID; //--- 세이더 프로그램 이름
	GLuint vertexShader; //--- 버텍스 세이더 객체
	GLuint fragmentShader; //--- 프래그먼트 세이더 객체
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



