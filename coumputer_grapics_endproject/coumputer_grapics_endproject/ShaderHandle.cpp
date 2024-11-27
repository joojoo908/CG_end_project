#include "ShaderHandle.h"

std::vector<Shader*> shaderList;
GLuint loc_modelMat = 0;
GLuint loc_PVM = 0;
GLuint loc_diffuseSampler = 0;
GLuint loc_normalSampler = 0;
GLuint loc_normalMat = 0;
GLuint loc_eyePos = 0;
GLuint loc_finalBonesMatrices = 0;

void GetShaderHandles()
{
    // Get shader handles
    loc_modelMat = shaderList[0]->GetModelMatLoc();
    loc_PVM = shaderList[0]->GetPVMLoc();
    loc_normalMat = shaderList[0]->GetNormalMatLoc();
    loc_eyePos = shaderList[0]->GetEyePosLoc();
    loc_finalBonesMatrices = shaderList[0]->GetFinalBonesMatricesLoc();
    loc_diffuseSampler = shaderList[0]->GetColorSamplerLoc();
    loc_normalSampler = shaderList[0]->GetNormalSamplerLoc();
}

glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
    return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}

// Shader creation
void CreateShader()
{
    Shader* shader = new Shader;
    shader->CreateFromFiles(vShaderPath, fShaderPath);
    shaderList.push_back(shader);
}