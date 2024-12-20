#pragma once
#include <vector>
#include <string>
#include <iostream>

#include <GL/glew.h>

#include <gl/glm/glm.hpp>
#include <gl/glm\gtc\matrix_transform.hpp>
#include <gl/glm\gtc\type_ptr.hpp>

#include "stb_image.h"

class Shader;
class Mesh;

class Skybox
{
public:
	Skybox(std::vector<std::string> faceLocations);

	void DrawSkybox(glm::mat4 viewMat, glm::mat4 projMat);

private:
	Mesh* skyMesh;
	Shader* skyShader;

	GLuint textureID;
	GLuint loc_PVM;
	GLuint loc_sampler;
};
