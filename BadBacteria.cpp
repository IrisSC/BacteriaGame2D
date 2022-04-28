#include "BadBacteria.h";
#include "ImageLoading.h"

#include "shaders\Shader.h"

#include <iostream>

BadBacteria::BadBacteria()
{
	m_vaoID = 0;
	m_vboID[0] = 0;
	m_vboID[1] = 0;
	m_Radius = 2.0f;

	health = 100;
	timeTilReplicate = 20000;
}

BadBacteria::BadBacteria(float x, float y, int t) {
	m_vaoID = 0;
	m_vboID[0] = 0;
	m_vboID[1] = 0;
	m_Radius = 2.0f;

	health = 100;
	xCenter = x;
	yCenter = y;
	timeTilReplicate = t;
}

void BadBacteria::SetRadius(float radius)
{
	m_Radius = radius;
}

void BadBacteria::SetXCenter(float x) {
	xCenter = x;
}

void BadBacteria::SetYCenter(float y) {
	yCenter = y;
}

void BadBacteria::SetHealth(int h) {
	health = h;
}

void BadBacteria::SetTime(int t) {
	timeTilReplicate = t;
}

float BadBacteria::GetRadius() {
	return m_Radius;
}

float BadBacteria::GetXCenter() {
	return xCenter;
}

float BadBacteria::GetYCenter() {
	return yCenter;
}

int BadBacteria::GetHealth() {
	return health;
}

int BadBacteria::GetTime() {
	return timeTilReplicate;
}

void BadBacteria::Init(Shader& shader, float colour[3], std::string filename)
{
	//load png image
	int imageHeight = 0;
	int imageWidth = 0;

	//create the texture on the GPU
	glGenTextures(1, &m_TexName);
	glBindTexture(GL_TEXTURE_2D, m_TexName);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);  //or use GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	bool success = ImageLoading::loadImage(filename);
	if (!success) {
		std::cout << "Unable to load image file for Player" << std::endl;
		glDeleteTextures(1, &m_TexName);
		return;
	}
	else
	{
		std::cout << "Image loaded " << std::endl;
	}

	//Create the geometry
	double step = 10;
	float size = (int)((360.0 / step) + 1);
	float* vert = new float[size * 3];
	float* col = new float[size * 3];
	float* tex = new float[size * 2];

	m_NumOfVerts = 0;
	for (double i = 0; i <= 360; i += step)
	{
		float xcoord = m_Radius * cos(glm::radians(i));
		float ycoord = m_Radius * sin(glm::radians(i));
		vert[m_NumOfVerts * 3] = xcoord;
		vert[m_NumOfVerts * 3 + 1] = ycoord;
		vert[m_NumOfVerts * 3 + 2] = 0.0;

		col[m_NumOfVerts * 3] = colour[0];
		col[m_NumOfVerts * 3 + 1] = colour[1];
		col[m_NumOfVerts * 3 + 2] = colour[2];

		tex[m_NumOfVerts * 2] = 0.5 + 0.5 * cos(glm::radians(i));
		tex[m_NumOfVerts * 2 + 1] = 0.5 + 0.5 * sin(glm::radians(i));

		m_NumOfVerts++;
	}

	//VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(2, m_vboID); // we need three VBOs - one for the vertices, one for the colours, and one for the textures

	//Lets set up the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);

	//initialises data storage of vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, m_NumOfVerts * 3 * sizeof(GLfloat), vert, GL_STATIC_DRAW);

	//set the position - linked to the position shader input
	GLint vertexLocation = glGetAttribLocation(shader.handle(), "in_Position");
	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Now set up the colours
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, m_NumOfVerts * 3 * sizeof(GLfloat), col, GL_STATIC_DRAW);

	//set the colour - linked to the colour shader input.
	GLint colorLocation = glGetAttribLocation(shader.handle(), "in_Color");
	glEnableVertexAttribArray(colorLocation);
	//location in shader, number of componentns,  type, normalised, stride, pointer to first attribute
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Now set up the texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[2]);
	glBufferData(GL_ARRAY_BUFFER, m_NumOfVerts * 3 * sizeof(GLfloat), tex, GL_STATIC_DRAW);

	//set the texture coords - linked to the texcoord shader input.
	GLint texLocation = glGetAttribLocation(shader.handle(), "in_TexCoord");
	glEnableVertexAttribArray(texLocation);
	//location in shader, number of componentns,  type, normalised, stride, pointer to first attribute
	glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//good practice to bind to 0.
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void BadBacteria::Render(Shader& shader, glm::mat4& ModelViewMatrix, glm::mat4& ProjectionMatrix)
{
	glUseProgram(shader.handle());  // use the shader

	//set the uniform for the projectionmatrix
	glUniformMatrix4fv(glGetUniformLocation(shader.handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//pass the uniform for the ModelView matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader.handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, m_TexName);
	//Draw the object
	//glPointSize(5.0);
	glBindVertexArray(m_vaoID);		// select first VAO
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_NumOfVerts);	// draw first object
	//glDrawArrays(GL_POINTS, 0, m_NumOfVerts);	// draw first object

	glBindVertexArray(0); //unbind the vertex array object
	glUseProgram(0); //turn off the current shader
}