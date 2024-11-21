#pragma once

#include "gl/glm/glm.hpp"

const int MAX_BONE_INFLUENCE = 4;

struct Vertex
{
	Vertex() {}

	Vertex(float px, float py, float pz)
	{
		Position.x = px; Position.y = py; Position.z = pz;
		TexCoords.s = 0.f; TexCoords.t = 0.f;
		Normal.x = 0.f; Normal.y = 0.f; Normal.z = 0.f;
	}

	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;

	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;

	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};