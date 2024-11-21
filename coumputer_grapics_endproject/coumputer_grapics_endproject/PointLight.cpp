#include "PointLight.h"

void PointLight::ShowProperties()
{
	
}

glm::mat4 PointLight::GetModelMat()
{
	return modelMat = glm::translate(glm::mat4(1.f), position);
}

void PointLight::UpdateTransform(glm::mat4 newModelMat)
{
	glm::vec3 translation, rotation, scale;
	//ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newModelMat), &translation[0], &rotation[0], &scale[0]);
	glm::value_ptr(newModelMat), & translation[0], & rotation[0], & scale[0];
	position = translation;
}
