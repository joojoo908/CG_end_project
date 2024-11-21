#pragma once
#include "Light.h"
class PointLight : public Light
{
public:
	PointLight(GLfloat ambient, GLfloat diffuse, glm::vec4 color, glm::vec3 position,
		GLfloat constant, GLfloat linear, GLfloat exponent) :
		Light(ambient, diffuse, color),
		position(position),
		constant(constant),
		linear(linear),
		exponent(exponent)
	{}

	virtual std::string GetName() override { return "PointLight"; }
	virtual void ShowProperties() override;
	virtual glm::mat4 GetModelMat() override;
	virtual void UpdateTransform(glm::mat4 newModelMat) override;

	glm::vec3 position;

	// attenuation
	GLfloat constant;
	GLfloat linear;
	GLfloat exponent;
};