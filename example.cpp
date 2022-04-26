#include <iostream>
using namespace std;

//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "GL\freeglut.h"
#include "shaders\Shader.h"
#include "Square.h"
#include "Circle.h"
#include "Player.h"
#include "BadBacteria.h"
#include "Sprite.h"

#include <map>
#include <list>
#include <iterator>

//included for getting time
#include <ctime>

#include <iostream>

using namespace std;

/*Variables for game Boundry*/
float xMax = 50.0f;
float yMax = 50.0f;
float xCamera = 0;
float yCamera = 0;

/*Variables for timing*/
float fDeltaTime = 0.0f;
float currentTicks = 0.0f;
float PreviousTicks = 0.0f;
/************************/

glm::mat4 ViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
int screenWidth = 500, screenHeight = 480;

Square myRedSquare, myGreenSquare;
Shader myShader;
Sprite background;
Player myPlayer;
map<int, BadBacteria> enemies;
BadBacteria enemy1(5.0f, 5.0f);
BadBacteria enemy2(1.0f, 1.0f);
BadBacteria enemy3(3.0f, 1.0f);
BadBacteria enemy4(0.0f, 0.0f);
int timeToReplicate = 0;
int numAddBB = 4;
int erase = -1;
float num = -5.0f;


//Variables for the positions of the squares
float XRedSquare = -50.0f;
float YRedSquare = 0;
float XGreenSquare = 5;
float YGreenSquare = 0;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;

float Angle = 0.0f;

#define PI 3.1415926535897932384626433832795
const double ToRadians = PI / 180.0;

//OPENGL FUNCTION PROTOTYPES
void display();				//used as callback in glut for display.
void reshape(int width, int height); //used as callback for reshape function in glut
void init();				//called in main when the program starts.

/*************    START OF OPENGL FUNCTIONS   ****************/

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width;
	screenHeight = height;

	glViewport(0, 0, width, height);						// set Viewport dimensions

	ProjectionMatrix = glm::ortho(-15.0, 15.0, -15.0, 15.0);
}


void display()
{ 
	//initials the objects for rendering 
	//init();
	//BadBacteria enemy1(5.0f, 5.0f);
	//obtain the ticks from the clock and find difference with previous time.
	currentTicks = std::clock();
	float deltaTicks = (float)(currentTicks - PreviousTicks);
	PreviousTicks = currentTicks;

	//now find the actual time passed between frames
	fDeltaTime = deltaTicks / (float)CLOCKS_PER_SEC;


	//clear the colour buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//set the camera x and y values
	if (!(myPlayer.GetXCenter() <= (-xMax + 15)) && !(myPlayer.GetXCenter() >= (xMax - 15))) {
		xCamera = myPlayer.GetXCenter();
	}
	if (!(myPlayer.GetYCenter() <= (-yMax + 15)) && !(myPlayer.GetYCenter() >= (yMax - 15))) {
		yCamera = myPlayer.GetYCenter();
	}
	//set the view matrix
	//ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-myPlayer.GetXCenter(), -myPlayer.GetYCenter(), 0.0));
	ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-xCamera, -yCamera, 0.0));

	//render the background
	background.Render(myShader, ViewMatrix, ProjectionMatrix);

	//set the modelViewMatrix for the player circle
	glm::mat4 playerTransform = glm::translate(ViewMatrix, glm::vec3(myPlayer.GetXCenter(), myPlayer.GetYCenter(), 0.0));
	glEnable(GL_BLEND);
		myPlayer.Render(myShader, playerTransform, ProjectionMatrix);
	glDisable(GL_BLEND);

	//set the modelviewmatrix for the green square
	/*glm::mat4 ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(XGreenSquare, YGreenSquare, 0.0));
	myGreenSquare.Render(myShader, ModelViewMatrix, ProjectionMatrix);*/

	//Set the modelviewtransform for the red square.
	glm::mat4 redTransform = glm::translate(ViewMatrix, glm::vec3(XRedSquare, YRedSquare, 0.0));
	myRedSquare.Render(myShader, redTransform, ProjectionMatrix);

	//replicate Bad Bacteria
	if (timeToReplicate == 10000) {
		//creates the new Bad Bacteria and adds them to a map
		map<int, BadBacteria> enemiesTemp;
		for (map<int, BadBacteria>::iterator it = enemies.begin(); it != enemies.end(); it++) {
			BadBacteria enemyN((*it).second.GetXCenter() + 1.0f, (*it).second.GetYCenter() + 1.0f);
			enemiesTemp.insert(std::pair<int, BadBacteria>(numAddBB, enemyN));
			numAddBB++;
		}
		//transfers the bad bacteria from the temperary map to the enemies map
		for (map<int, BadBacteria>::iterator it = enemiesTemp.begin(); it != enemiesTemp.end(); it++) {
			BadBacteria tempBB = (*it).second;
			int tempNum = (*it).first;
			enemies.insert(std::pair<int, BadBacteria>(tempNum, tempBB));
			float red[3] = { 1, 0, 0 };
			tempBB.SetRadius(2.0f);
			tempBB.Init(myShader, red, "textures/BadBacteriaTransparent.png");
		}
		//resets the frames to replace
		timeToReplicate = 0;
	}
	else {
		//increases time to replaces by one
		timeToReplicate++;
		//cout << timeToReplicate << "  ";
	}

	//Set the modelview transform for the emeny bacteria
	for (map<int, BadBacteria>::iterator it = enemies.begin(); it != enemies.end(); it++) {
		//cout << " inside display for loop ";
		BadBacteria create = (*it).second;
		float xLocation = create.GetXCenter();
		//cout << xLocation;
		// check if Bad Bacteria is close enough to player to run away
		if ((myPlayer.GetXCenter() - create.GetXCenter()) * (myPlayer.GetXCenter() - create.GetXCenter()) +
			(myPlayer.GetYCenter() - create.GetYCenter()) * (myPlayer.GetYCenter() - create.GetYCenter()) <
			(myPlayer.GetRadius() + create.GetRadius()) * (myPlayer.GetRadius() + create.GetRadius()) + 100) {
			//BB move away
			float xMovement = (*it).second.GetXCenter() - myPlayer.GetXCenter();
			float yMovement = (*it).second.GetYCenter() - myPlayer.GetYCenter();
			if ((*it).second.GetXCenter() < xMax-2.0f && (*it).second.GetXCenter() > -xMax + 2.0f) {
				(*it).second.SetXCenter((*it).second.GetXCenter() + xMovement /150);
			}
			if ((*it).second.GetYCenter() < yMax - 2.0f && (*it).second.GetYCenter() > -yMax + 2.0f) {
				(*it).second.SetYCenter((*it).second.GetYCenter() + yMovement/150);
			}
		}
		bool getNext = false;
		for (map<int, BadBacteria>::iterator it2 = it; it2 != enemies.end(); it2++) {
			if (getNext == false) {
				getNext = true;
			}
			else {
				//cout << "inside second for loop" << (*it2).first;
				//check if it is in contact with any niebors, if so move
				BadBacteria next = (*it2).second;
				if ((next.GetXCenter() - create.GetXCenter()) * (next.GetXCenter() - create.GetXCenter()) +
					(next.GetYCenter() - create.GetYCenter()) * (next.GetYCenter() - create.GetYCenter()) <
					(next.GetRadius() + create.GetRadius()) * (next.GetRadius() + create.GetRadius())) {
					float xMovement = (*it).second.GetXCenter() - next.GetXCenter();
					float yMovement = (*it).second.GetYCenter() - next.GetYCenter();
					if ((*it).second.GetXCenter() < xMax - 2.0f && (*it).second.GetXCenter() > -xMax + 2.0f) {
						(*it).second.SetXCenter((*it).second.GetXCenter() + xMovement / 10000);
					}
					if ((*it).second.GetYCenter() < yMax - 2.0f && (*it).second.GetYCenter() > -yMax + 2.0f) {
						(*it).second.SetYCenter((*it).second.GetYCenter() + yMovement / 10000);
					}
				}
			}
		}
		//check if in collition with Player
		if ((myPlayer.GetXCenter() - create.GetXCenter())*(myPlayer.GetXCenter() - create.GetXCenter())+ 
			(myPlayer.GetYCenter() - create.GetYCenter()) * (myPlayer.GetYCenter() - create.GetYCenter()) < 
			(myPlayer.GetRadius() + create.GetRadius())* (myPlayer.GetRadius() + create.GetRadius())) {
			//minus one from health of BB
			(*it).second.SetHealth((*it).second.GetHealth() - 1);
			cout << " minus one health" << (*it).second.GetHealth();
		}
		if (create.GetHealth() <= 0) {
			//saves key to erase when out of loop
			erase = (*it).first;
		}
		else {
			glm::mat4 enemyTransform = glm::translate(ViewMatrix, glm::vec3(create.GetXCenter(), create.GetYCenter(), 0.0));
			glEnable(GL_BLEND);
				create.Render(myShader, enemyTransform, ProjectionMatrix);
			glDisable(GL_BLEND);
		}
		
	}
	//delete BB whos health is depleted
	if (erase != -1) {
		enemies.erase(erase);
		erase = -1;
	}

	glutSwapBuffers();

	Angle += 0.0005f;
	if (Angle >= 360)
		Angle = 0;

}

void init()
{
	if (!myShader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}


	glClearColor(1.0, 1.0, 1.0, 0.0);						//sets the clear colour to black
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/*if (!myShader.load("Basic", "./glslfiles/basicTransformations.vert", "./glslfiles/basicTransformations.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}*/
	

	myRedSquare.SetSideSize(4.0f);
	float red[3] = { 1,0,0 };
	myRedSquare.Init(myShader, red);


	//create background
	background.SetWidth(xMax*2.0f);
	background.SetHeight(yMax*2.0f);
	background.Init(myShader, red, "textures/Background.png");
	
	/*myGreenSquare.SetSideSize(3.0f);
	float green[3] = { 0,1,0 };
	myGreenSquare.Init(myShader, green);*/

	//add the Player
	myPlayer.SetRadius(4.0f);
	float green[3] = { 0, 1, 0 };
	
	myPlayer.Init(myShader, green, "textures/Player2Transparent2.png");
	
	//add the enemies
	for (map<int, BadBacteria>::iterator it = enemies.begin(); it != enemies.end(); it++) {
		//cout << "inside init loop" << (*it).second.GetXCenter();
		(*it).second.SetRadius(2.0f);
		(*it).second.Init(myShader, red, "textures/BadBacteriaTransparent.png");
	}
	
	enemy1.SetRadius(2.0f);
	enemy1.Init(myShader, red, "textures/BadBacteriaTransparent.png");
	
	//enemy2.SetRadius(2.0f);
	//enemy2.Init(myShader, red);
}

//render new Bad Bacteria
/*void initNewBB(BadBacteria newBB) {
	float red[3] = { 1,0,0 };
	newBB.SetRadius(2.0f);
	newBB.Init(myShader, red);
}*/

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	}
}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	}
}

void processKeys()
{
	if (Left)
	{
		//check to see if out of bounds
		if (myPlayer.GetXCenter()-0.01f-myPlayer.GetRadius() > -xMax) {
			//scale the speed based on the time between frames to obtain distance to move this frame.
			myPlayer.SetXCenter((myPlayer.GetXCenter() - 0.01f));
		}
	}
	if (Right)
	{
		//check: make sure in bounds
		if (myPlayer.GetXCenter() + 0.01f + myPlayer.GetRadius() < xMax) {
			myPlayer.SetXCenter((myPlayer.GetXCenter() + 0.01f));
			//XRedSquare += 10.0 * fDeltaTime;
		}
	}
	if (Up)
	{
		//check: make sure in bounds
		if (myPlayer.GetYCenter() + 0.01f + myPlayer.GetRadius() < yMax) {
			myPlayer.SetYCenter((myPlayer.GetYCenter() + 0.01f));
			//YRedSquare += 10.0 * fDeltaTime;
		}
		
	}
	if (Down)
	{
		//Check: make sure in bounds
		if (myPlayer.GetYCenter() - 0.01f - myPlayer.GetRadius() > -yMax) {
			myPlayer.SetYCenter((myPlayer.GetYCenter() - 0.01f));
			//YRedSquare -= 10.0 * fDeltaTime;
		}
		
	}
}

void idle()
{
	processKeys();

	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

// FREEGLUT WINDOW SET UP
int main(int argc, char** argv)
{
	enemies.insert(std::pair<int, BadBacteria>(1, enemy1));
	enemies.insert(std::pair<int, BadBacteria>(2, enemy2));
	enemies.insert(std::pair<int, BadBacteria>(3, enemy3));

	//replicate Bad Bacteria
	/*if (timeToReplicate >= 200) {
		for (map<int, BadBacteria>::iterator it = enemies.begin(); it != enemies.end(); it++) {
			BadBacteria newBB((*it).second.GetXCenter(), (*it).second.GetYCenter());
			enemies.insert(std::pair<float, BadBacteria>(3, newBB));
		}
	}
	else {
		timeToReplicate++;
		cout << timeToReplicate << "  ";
	}*/

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL FreeGLUT Example: Square");

	glutReshapeFunc(reshape);

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	//initialise the objects for rendering
	init();

	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);


	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}

