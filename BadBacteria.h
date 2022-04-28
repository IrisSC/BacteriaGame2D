#pragma once

#include "shaders\Shader.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"


class BadBacteria
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[2];	// ids for Vertex Buffer Objects

	float m_Radius;
	int m_NumOfVerts;
	int health;
	int timeTilReplicate;

	float xCenter;
	float yCenter;

	GLuint m_TexName; //identifier for the texture
public:
	BadBacteria();
	BadBacteria(float x, float y, int t);
	void SetRadius(float r);
	void SetXCenter(float x);
	void SetYCenter(float y);
	void SetHealth(int h);
	void SetTime(int t);
	float GetRadius();
	float GetXCenter();
	float GetYCenter();
	int GetHealth();
	int GetTime();

	void Init(Shader& shader, float colour[3],std::string filename);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};