#pragma once

#include "Shader.h"

// Vertex Shader
static const char* vShaderPath = "Shader/vertex.glsl";
// Fragment Shader
static const char* fShaderPath = "Shader/fragment.glsl";

extern std::vector<Shader*> shaderList;
extern GLuint loc_modelMat;
extern GLuint loc_PVM;
extern GLuint loc_diffuseSampler;
extern GLuint loc_normalSampler;
extern GLuint loc_normalMat;
extern GLuint loc_eyePos;
extern GLuint loc_finalBonesMatrices;


// Shader handles
void GetShaderHandles();

glm::mat3 GetNormalMat(glm::mat4& modelMat);

// Shader creation
void CreateShader();