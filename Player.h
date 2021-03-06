#pragma once

#include "shaders\Shader.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "gl\glew.h"
#include <string>

class Player
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

	float m_Radius;
	int m_NumOfVerts;

	float xCenter;
	float yCenter;

	GLuint m_TexName; //identifier for the texture

public:
	Player();					//default constructor
	void SetRadius(float radius);
	void SetXCenter(float x);
	void SetYCenter(float y);
	float GetRadius();
	float GetXCenter();
	float GetYCenter();
	void Init(Shader& shader, float colour[3], std::string filename);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};