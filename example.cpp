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
#include <ctime>

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
Sprite background, startScreen, endCongrats, endDeath, numBar, numBarOutline;
Player myPlayer;
map<int, BadBacteria> enemies;
int xMaxInt = (int)xMax;
int yMaxInt = (int)yMax;
//srand(time(NULL));
BadBacteria enemy1(0.0f, 0.0f, 0);
BadBacteria enemy2(0.0f, 0.0f, 0);
BadBacteria enemy3(0.0f, 0.0f, 0); 
BadBacteria enemy4(0.0f, 0.0f, 0);
int timeToReplicate = 0;
int numAddBB = 4;
int erase = -1;
float num = -5.0f;

//determines wether the start/end screens will display
bool startScreenShow = true;
bool endCongratsShow = false;
bool endDeathShow = false;

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

	ProjectionMatrix = glm::ortho(-30.0, 30.0, -30.0, 30.0); 
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
	if (!(myPlayer.GetXCenter() <= (-xMax + 30)) && !(myPlayer.GetXCenter() >= (xMax - 30))) {
		xCamera = myPlayer.GetXCenter();
	}
	if (!(myPlayer.GetYCenter() <= (-yMax + 30)) && !(myPlayer.GetYCenter() >= (yMax - 30))) {
		yCamera = myPlayer.GetYCenter();
	}

	//set the view matrix
	//ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-myPlayer.GetXCenter(), -myPlayer.GetYCenter(), 0.0));
	ViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-xCamera, -yCamera, 0.0));

	//check if the start screen should be shown
	if (startScreenShow) {
		//display the start screen
		glm::mat4 startScreenTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
		startScreen.Render(myShader, startScreenTransform, ProjectionMatrix, 1);
	}

	//check if the congradulations end screen should display
	if (endCongratsShow) {
		//display the congradulations end screen
		glm::mat4 endScreenCongratsTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
		endCongrats.Render(myShader, endScreenCongratsTransform, ProjectionMatrix, 1);
	}

	//check if the death end screen should be displayed
	if (endDeathShow) {
		//display the death end screen
		glm::mat4 endScreenDeathTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
		endDeath.Render(myShader, endScreenDeathTransform, ProjectionMatrix, 1);
	}

	//check if the game should be displayed
	if (!startScreenShow && !endCongratsShow && !endDeathShow) {
		//render the background
		background.Render(myShader, ViewMatrix, ProjectionMatrix, 1);

		//render number of bacteria bar
		//numBar.SetWidth(enemies.size());
		//float green[] = {0,0,1};
		//numBar.Init(myShader, green, "textures/sky.png", 1.0, 1.0);
		//glm::mat4 testing = glm::translate(ViewMatrix, glm::vec3(xCamera - 4.0, yCamera + 28.0, 0.0));
		//numBarOutline.Render(myShader, testing, ProjectionMatrix, 0);
		//numBar.Render(myShader, ViewMatrix, ProjectionMatrix);

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
		/*if (timeToReplicate == 2000) {
			//creates the new Bad Bacteria and adds them to a map
			map<int, BadBacteria> enemiesTemp;
			for (map<int, BadBacteria>::iterator it = enemies.begin(); it != enemies.end(); it++) {
				BadBacteria enemyN((*it).second.GetXCenter() + 1.0f, (*it).second.GetYCenter() + 1.0f, 20000);
				enemiesTemp.insert(std::pair<int, BadBacteria>(numAddBB, enemyN));
				numAddBB++;
			}
			//transfers the bad bacteria from the temperary map to the enemies map
			for (map<int, BadBacteria>::iterator it = enemiesTemp.begin(); it != enemiesTemp.end(); it++) {
				BadBacteria tempBB = (*it).second;
				int tempNum = (*it).first;
				//renders the Bad Bacteria
				float red[3] = { 1, 0, 0 };
				tempBB.SetRadius(2.0f);
				tempBB.Init(myShader, red, "textures/BadBacteriaTransparent.png");

				enemies.insert(std::pair<int, BadBacteria>(tempNum, tempBB));
			}
			//resets the frames to replace
			timeToReplicate = 0;
		}
		else {
			//increases time to replaces by one
			timeToReplicate++;
			cout << timeToReplicate << "  ";
		}
		*/
		float barPlacement = -29.0f;
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
					(*it).second.SetXCenter((*it).second.GetXCenter() + xMovement /2000);
				}
				if ((*it).second.GetYCenter() < yMax - 2.0f && (*it).second.GetYCenter() > -yMax + 2.0f) {
					(*it).second.SetYCenter((*it).second.GetYCenter() + yMovement/2000);
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
							(*it).second.SetXCenter((*it).second.GetXCenter() + xMovement / 500);
						}
						if ((*it).second.GetYCenter() < yMax - 2.0f && (*it).second.GetYCenter() > -yMax + 2.0f) {
							(*it).second.SetYCenter((*it).second.GetYCenter() + yMovement / 500);
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
			//check if the Bacteria should replicate
			if ((*it).second.GetTime() <= 0) {
				//put in seed for random
				srand(time(NULL));

				//create new bacteria
				BadBacteria enemyN((*it).second.GetXCenter() + 1.0f, (*it).second.GetYCenter() + 1.0f, rand() % (20000 - 5000) + 5000);
				float red[3] = { 1, 0, 0 };
				enemyN.SetRadius(2.0f);
				enemyN.Init(myShader, red, "textures/BadBacteriaTransparent.png");
				enemies.insert(std::pair<int, BadBacteria>(numAddBB, enemyN));
				numAddBB++;

				//reset old bacteria time
				(*it).second.SetTime(rand() % (20000 - 5000) + 5000);
			}
			else {
				(*it).second.SetTime((*it).second.GetTime() - 1);
			}
			//add to the Bacteria number bar
			glm::mat4 bar = glm::translate(ViewMatrix, glm::vec3(xCamera + barPlacement, yCamera + 28.0, 0.0));
			numBar.Render(myShader, bar, ProjectionMatrix, 1);
			barPlacement = barPlacement + 0.5f;
		}
		//delete BB whos health is depleted
		if (erase != -1) {
			enemies.erase(erase);
			erase = -1;
		}
		//displey the outline for the number bar
		glm::mat4 testing = glm::translate(ViewMatrix, glm::vec3(xCamera - 4.0, yCamera + 28.0, 0.0));
		numBarOutline.Render(myShader, testing, ProjectionMatrix, 0);
	}
	
	//check if player has killed all enemies
	if (enemies.size() <= 0) {
		endCongratsShow = true;
	}

	//check if there are too many bacteria
	if (enemies.size() >= 100) {
		endDeathShow = true;
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
	
	//add colors
	float green[3] = { 0, 1, 0 };
	float red[3] = { 1,0,0 };

	myRedSquare.SetSideSize(4.0f);
	myRedSquare.Init(myShader, red);


	//create background
	background.SetWidth(xMax*2.0f);
	background.SetHeight(yMax*2.0f);
	background.Init(myShader, red, "textures/Background.png", 2.0, 2.0);

	//create start screen
	startScreen.SetWidth(60);
	startScreen.SetHeight(60);
	startScreen.Init(myShader, red, "textures/StartScreen3.png", 1.0, 1.0);

	//create congrats end screen
	endCongrats.SetWidth(60);
	endCongrats.SetHeight(60);
	endCongrats.Init(myShader, red, "textures/EndScreenCongrats1.png", 1.0, 1.0);

	//create the death end screen
	endDeath.SetWidth(60);
	endDeath.SetHeight(60);
	endDeath.Init(myShader, red, "textures/EndScreen2.png", 1.0, 1.0);
	
	/*myGreenSquare.SetSideSize(3.0f);
	float green[3] = { 0,1,0 };
	myGreenSquare.Init(myShader, green);*/

	//add the Player
	myPlayer.SetRadius(4.0f);
	myPlayer.Init(myShader, green, "textures/Player2Transparent2.png");

	//add bacteria num bar
	numBar.SetWidth(0.5f);
	numBar.SetHeight(2.0f);
	numBar.Init(myShader, green, "textures/green2.png", 1.0, 1.0);

	//add the outline for the bacteria number bar
	numBarOutline.SetWidth(50.0);
	numBarOutline.SetHeight(2.0);
	numBarOutline.Init(myShader, green, "textures/green2.png", 1.0, 1.0);
	
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

void keyfunction(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		startScreenShow = false;
		break;
	}
}

/*void keyfunctionUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		startScreenShow = false;
		break;
	}
}*/

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
	srand(time(NULL));

	//set the enmies to appear randomly
	enemy1.SetXCenter((float)(rand() % ((xMaxInt - 2) - (-(xMaxInt - 2))) + -(xMaxInt - 2)));
	enemy1.SetYCenter((float)(rand() % ((yMaxInt - 2) - (-(yMaxInt - 2))) + -(yMaxInt - 2)));
	enemy1.SetTime(rand() % (20000 - 5000) + 5000);

	enemy2.SetXCenter((float)(rand() % ((xMaxInt - 2) - (-(xMaxInt - 2))) + -(xMaxInt - 2)));
	enemy2.SetYCenter((float)(rand() % ((yMaxInt - 2) - (-(yMaxInt - 2))) + -(yMaxInt - 2)));
	enemy2.SetTime(rand() % (20000 - 5000) + 5000);

	enemy3.SetXCenter((float)(rand() % ((xMaxInt - 2) - (-(xMaxInt - 2))) + -(xMaxInt - 2)));
	enemy3.SetYCenter((float)(rand() % ((yMaxInt - 2) - (-(yMaxInt - 2))) + -(yMaxInt - 2)));
	enemy3.SetTime(rand() % (20000 - 5000) + 5000);

	enemies.insert(std::pair<int, BadBacteria>(1, enemy1));
	cout << "X random center: " << rand()%50 << "stop random ";
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

	glutKeyboardFunc(keyfunction);
	//glutKeyboardUpFunc(keyfunctionUp);


	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}

