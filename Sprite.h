#pragma once


#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "gl\glew.h"
#include "shaders\Shader.h"
#include <string>

class Sprite
{
private:
	unsigned int m_vaoID;		// id for Vertex Array Object
	unsigned int m_vboID[3];	// ids for Vertex Buffer Objects
	unsigned int m_NumberOfVerts;//number of vertices in the object

	//Dimensions of the sprite.
	float m_Width;
	float m_Height;

	GLuint m_TexName; //identifier for the texture

public:
	Sprite();					//default constructor
	void Init(Shader& shader, float colour[3], std::string filename, float repeatInS, float
		repeatInT);
	void SetWidth(float size);
	void SetHeight(float size);
	void Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix);
};