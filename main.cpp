
// Code Written by Alan Jaison for EEE2041
//!Includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <Shader.h>
#include <Vector.h>
#include <Matrix.h>
#include <Mesh.h>
#include <Texture.h>
#include <SphericalCameraManipulator.h>
#include <iostream>
#include <math.h>
#include <string>



//!Function Prototypes
bool initGL(int argc, char** argv);
void display(void);
void keyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void handleKeys();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void Timer(int value);
void initTexture(std::string filename, GLuint & textureID);
void initShader();					                //Function to init Shader
float toRadian(float degree);
void render2dText(std::string text, float r, float g, float b, float x, float y);
void handlelevel();



//! Screen size
int screenWidth = 720;
int screenHeight = 720;

//! Array of key states
bool keyStates[256];

//Global Variables
GLuint shaderProgramID;			                    // Shader Program ID
GLuint vertexPositionAttribute;		                // Vertex Position Attribute Location
GLuint vertexTexcoordAttribute;// Vertex Texcoord Attribute Location
GLuint TextureMapUniformLocation; // Texture Map Location
GLuint vertexNormalAttribute;
GLuint texture; // OpenGL Texture   //PLANE
GLuint texture2; // OpenGL Texture  //COIN
GLuint texture3; // OpenGL Texture  //GRASS
GLuint texture4; // OpenGL Texture  //CRATE
GLuint texture5;
GLuint texture6; // gravel
Mesh mesh;	//PLANE			                  
Mesh mesh1; //COIN
Mesh mesh2; //GRASS
Mesh mesh3; //TORUS
Mesh mesh4; //building


//Global variables for scene illumination 
//Vector3f lightPosition;				                // Light Position 
//Material Properties
//Vector3f lightPosition;				                // Light Position 
GLuint LightPositionUniformLocation;                // Light Position Uniform   
Vector3f colour;					                // Colour Variable 
GLuint ColourUniformLocation;		                // Colour Uniform - Explained in later excercise

GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;

Vector3f lightPosition = Vector3f(5, 5, 5);  // Light Position 
Vector3f ambient = Vector3f(0.15, 0.1, 0.1);
Vector3f specular = Vector3f(25, 7,0.0);
float specularPower = 200;
//Skybox skybox;

//Viewing
SphericalCameraManipulator cameraManip;
Matrix4x4 ModelViewMatrix;		                    // ModelView Matrix
GLuint MVMatrixUniformLocation;		                // ModelView Matrix Uniform
Matrix4x4 ProjectionMatrix;		                    // Projection Matrix
GLuint ProjectionUniformLocation;	                // Projection Matrix Uniform Location

//Plane Position Global Variables
float t = 0;
int forward = 0;
int planerot = 0;
float planemovex = 20;
float planemovey = 0;
float planemovez = 20;
float planerotation = 0;
float rollrotation = 0;
float speed = 0;
float acceleration = 0.00017;
float planeheightrotation = 0;
int downpressed = 0;
int uppressed = 0;

int showmenu = 1;
int reset = 0;
int score = 0;

//Camera view variable
int cockpitview = 0;


// variables for scene 
int level = 1;
#define xlength 20
#define zlength 20
int map[zlength][xlength];
int yposition = 0;
int coinrotation;
int togglelight = 0;

//For computer generated level 

int randomx = 0;
int randomy = 0;
int randomz = 0;
float randomrot = (rand() % 88);

int time;


//! Main Program Entry
int main(int argc, char** argv)
{
	//init OpenGL
	if (!initGL(argc, argv))
		return -1;

	//Init Key States to false;    
	for (int i = 0; i < 256; i++)
		keyStates[i] = false;


	initShader();

	
	//Set up your program
	mesh.loadOBJ("../models/plane1.obj");
	mesh1.loadOBJ("../models/coin.obj");
	mesh2.loadOBJ("../models/cube.obj");
	mesh3.loadOBJ("../models/torus.obj");
	mesh4.loadOBJ("../models/Deer_type_03_A_OBJ.obj");

	//Init Camera Manipultor
	cameraManip.setPanTiltRadius(0.f, 0.f, 2.f);
	cameraManip.setFocus(mesh.getMeshCentroid());

	initTexture("../models/plane1.bmp", texture);
	initTexture("../models/coin.bmp", texture2);
	initTexture("../models/grass.bmp", texture3);
	initTexture("../models/Crate.bmp", texture4);
	initTexture("../models/Grass01.bmp", texture5);
	initTexture("../models/gr.bmp", texture6);

	//Set colour variable and light position
	//Set colour variable and light position
	colour = Vector3f(1.0, 0.0, 1.0);
	//lightPosition = Vector3f(20.0, 20.0, 20.0);


	//Enter main loop
	glutMainLoop();

	//Delete shader program
	glDeleteProgram(shaderProgramID);

	return 0;
}

// TEXTURE FUNCTION

void initTexture(std::string filename, GLuint & textureID)
{

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Get texture Data
	int width, height;
	char*data;
	Texture::LoadBMP(filename, width, height, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete[] data;
}

//! Function to Initialise OpenGL
bool initGL(int argc, char** argv)
{
	//Init GLUT
	glutInit(&argc, argv);

	//Set Display Mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	//Set Window Size
	glutInitWindowSize(screenWidth, screenHeight);

	// Window Position
	glutInitWindowPosition(250, 70);

	//Create Window
	glutCreateWindow("Plane Assignment");

	// Init GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	//Set Display function
	glutDisplayFunc(display);

	//Set Keyboard Interaction Functions
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);

	//Set Mouse Interaction Functions
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	glutMotionFunc(motion);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	//Start start timer function after 100 milliseconds
	glutTimerFunc(100, Timer, 0);

	return true;
}


//! Display Loop
void display(void)
{
	//Handle keys
	handleKeys();

	//Set Viewport
	glViewport(0, 0, screenWidth, screenHeight);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw your scene



	//To make the background sky blue
	if ((level == 1) || level == 2)
	{
		glClearColor(0.2, 0.6, 1.0, 1.0);
	}
	else
	{
		glClearColor(1.0, 0.0, 0.1, 1.0);
	}

	glUseProgram(shaderProgramID);

	char timechar[20];
	char levelchar[20];
	int time = 100 - t;
	sprintf_s(timechar, "Time: %i", time);
	render2dText(timechar, 1.0, 1.0, 1.0, -1.0, 0.95);
	render2dText("Score: 0" , 1.0, 1.0, 1.0, -1.0, 0.90);
	sprintf_s(levelchar, "Level: %i", level);
	render2dText(levelchar, 1.0, 1.0, 1.0, -0.13, 0.95);


	if (showmenu == 1)
	{
		render2dText("Alan Jaison's Flight Simulator", 1.0, 0.0, 0.0, -0.3, 0.55);
		render2dText("(w,a,s,d) Forwards, Backwards, Turn Left, Right", 1.0, 0.0, 0.0, -0.3, -0.15);
		render2dText("(j,l) Roll left , Roll right", 1.0, 0.0, 0.0, -0.30, -0.25);
		render2dText("(i,k) Roll up , Roll down", 1.0, 0.0, 0.0, -0.30, -0.35);
		render2dText("(t) Toggle shaders, normal, zen mode", 1.0, 0.0, 0.0, -0.30, -0.45);
		render2dText("(f) Toggle fog, on/off", 1.0, 0.0, 0.0, -0.30, -0.55);
	}

	//Projection Matrix - Perspective Projection
	ProjectionMatrix.perspective(90, 1.0, 0.0001, 100.0);

	//Set Projection Matrix
	glUniformMatrix4fv(
		ProjectionUniformLocation,  //Uniform location
		1,							//Number of Uniforms
		false,						//Transpose Matrix
		ProjectionMatrix.getPtr());	//Pointer to ModelViewMatrixValues

	//Apply Camera Manipluator to Set Model View Matrix on GPU
	ModelViewMatrix.toIdentity();
	Matrix4x4 m = cameraManip.apply(ModelViewMatrix);
	//m.translate(planemovex, 5.0f, planemovez);
	glUniformMatrix4fv(
		MVMatrixUniformLocation,  	//Uniform location
		1,					        //Number of Uniforms
		false,				        //Transpose Matrix
		m.getPtr());	        //Pointer to Matrix Values



	//Increment variable to rotate ring
	coinrotation++;


	//Plane movement
	if (forward == 0)
	{
	
		speed = 0;
	
	}

	if (forward == 1)
	{
		if (speed <= 0.1)
		{
			speed = speed + acceleration;
		}
		planemovez += speed * cosf(toRadian(planerotation));
		planemovey -= speed * tanf(toRadian(planeheightrotation));
		planemovex += speed * sinf(toRadian(planerotation));
	}

	if (forward == -1)
	{
		planemovex -= 0.05 * sinf(toRadian(planerotation));
		planemovez -= 0.05 * cosf(toRadian(planerotation));
	}

	//------------------------------------------------------------------
	// Plane y-axis up down flight rotation

	/*if ((speed >= 0.034) && (downpressed == 1))
	{
		//plane moves up
		planemovey += speed * cosf(toRadian(planeheightrotation));
	}*/

	if (uppressed == 1)
	{
		//plane moves up
		planemovey -= 0.02 * sinf(toRadian(planeheightrotation));
	}


	printf("reset = %i\n", reset);
	//plane and camera reset 
	if (reset == 1)
	{
		forward = 0;
		planerot = 0;
		planemovex = 20;
		planemovey = 0;
		planemovez = 20;
		planerotation = 0;
		rollrotation = 0;
		speed = 0;
		acceleration = 0.00017;
		planeheightrotation = 0;
		downpressed = 0;
		uppressed = 0;
		score = 0;
		time = 100;
		t = 0;


		showmenu = 1;
		reset = 0;
	}

	//Seting camera to follow and and focus on the plane

	if (cockpitview == 0)
	{
		cameraManip.setFocus(Vector3f((planemovex), planemovey, planemovez));
		cameraManip.setPanTiltRadius((float)(planerotation * 0.0175), -0.7f, 3.5f);

	}
	
	if (cockpitview == 1)
	{
		cameraManip.setFocus(Vector3f((planemovex), planemovey-0.5, planemovez+5.5));
		cameraManip.setPanTiltRadius((float)(planerotation * 0.0175), -1.0f, -5.0f);
	}
	//------------------------------------------------------------------
	//Call Draw Geometry Function
	//Set Colour after program is in use



	//Boundries 
	if (((planemovex >= 45) || (planemovez >= 45)) || ((planemovex <= -5) || (planemovez <= -5) || (planemovey <= -0.2) || (planemovey >= 60)))
	{
		reset = 1;
		showmenu = 1;
	}
	//Plane
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(TextureMapUniformLocation, 0);
	Matrix4x4 plane = cameraManip.apply(ModelViewMatrix);
	plane.translate(planemovex, planemovey, planemovez);
	plane.scale(0.5f, 0.5f, 0.5f);
	//along y axis so left right
	plane.rotate((float)(planerotation), 0.f, 1, 0.f);
	//along x axis so up down
	plane.rotate((float)(planeheightrotation), 1, 0.f, 0.f);
	//zaxis
	plane.rotate((float)(rollrotation), 0.0f, 0.f, 1);
	glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, plane.getPtr());
	mesh.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);

	// Level 1 or 2

	
		//FLOOR
		glActiveTexture(GL_TEXTURE0);
		if ((level == 1) || (level == 2))
		{
			glBindTexture(GL_TEXTURE_2D, texture3);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texture6 );
		}

		glUniform1i(TextureMapUniformLocation, 4);

		
		Matrix4x4 floor = cameraManip.apply(ModelViewMatrix);
		floor.translate(20.0f, -1.05f, 20.0f);
		floor.scale(25.0f, 0.15f, 25.0f);
		glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, floor.getPtr());
		mesh2.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);


	//Drawing Different scenes 

	handlelevel();

	for (int i = 0; i < zlength; i++)
	{
		for (int j = 0; j < xlength; j++)
		{
			if (map[i][j] == 1)
			{
				//Ring
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture4);
				glUniform1i(TextureMapUniformLocation, 0);

				Matrix4x4 torus = cameraManip.apply(ModelViewMatrix);
				torus.translate((float)(i * 2), (float)(yposition), (float)(j * 2));
				torus.rotate((float)90, 1, 0.0f, 0.0f);
				torus.scale(2.1f, 2.1f, 2.1f);
				glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, torus.getPtr());
				mesh3.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);

			}

			  //Loading in coins
			if (map[i][j] == 2)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture2);
				glUniform1i(TextureMapUniformLocation, 0);
				Matrix4x4 coin = cameraManip.apply(ModelViewMatrix);
				coin.translate((float)(i * 2), (float)(yposition), (float)(j * 2));
				coin.scale(0.9f, 0.9f, 0.9f);
				coin.rotate(coinrotation, 0.0, 1.0, 0.0);
				glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, coin.getPtr());
				mesh1.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);
			}

			if (map[i][j] == 3)
			{
				//-------------------------------------------------------------------Ring
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture4);
				glUniform1i(TextureMapUniformLocation, 0);

				Matrix4x4 torus = cameraManip.apply(ModelViewMatrix);
				torus.translate((float)(i * 2), (float)(yposition), (float)(j * 2));
				torus.rotate((randomrot + i * 5), 1, 0.0f, 0.0f);
				torus.rotate((randomrot + j * 5), 1, 0.0f, 0.0f);
				torus.rotate((randomrot + i * 3), 0.0f, 1, 0.0f);
				//torus.rotate((float)(randomrot), 1, 0.0f, 0.0f);
				torus.scale(2.1f, 2.1f, 2.1f);
				glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, torus.getPtr());
				mesh3.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);


				//deer
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture4);
				glUniform1i(TextureMapUniformLocation, 0);
				Matrix4x4 building = cameraManip.apply(ModelViewMatrix);
				building.translate(40.0f, -0.65f, 20.0f);
				building.scale(0.06f, 0.06f, 0.06f);
				glUniformMatrix4fv(MVMatrixUniformLocation, 1, false, building.getPtr());
				mesh4.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute);

			}

		}
	}


	//Score check



/*	printf("planemovex = %i\n", planemovex);
	printf("planemovez = %i\n", planemovez);

	for (int i = 0; i < zlength; i++)
	{
		for (int j = 0; j < xlength; j++)
		{
			if (map[i][j] == 2)
			{
				if (
					((planemovex < j + 3) && (planemovex > j - 3)) &&
					((planemovez < i + 3) && (planemovez > i - 3)))
				{
					score += 1;

				}
			}
		}
	} */

	/*int tempy;
	int tempx;
	int tempcoiny;
	int tempcoinx;

	tempcoiny = (planemovex / 2);
	tempcoinx = (planemovez / 2);
	if (fmod(planemovex, 2) > 1) tempcoiny++;
	if (fmod(planemovez, 2) > 1) tempcoinx++;

	//Coin detection
	if (map[tempcoiny][tempcoinx] == 2)
	{
		map[tempcoinx][tempcoiny] = 1;
	    score++;
	} */


	

	if (togglelight == 0)
	{
		glUniform3f(ColourUniformLocation, colour.x, colour.y, colour.z);
		glUniform3f(LightPositionUniformLocation, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 1.0);
		glUniform4f(SpecularUniformLocation, specular.x, specular.y, specular.z, 1.0);
		glUniform1f(SpecularPowerUniformLocation, specularPower);

		vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");
		TextureMapUniformLocation = glGetUniformLocation(shaderProgramID, "TextureMap_uniform");
		vertexNormalAttribute = glGetAttribLocation(shaderProgramID, "aVertexNormal");
		ColourUniformLocation = glGetUniformLocation(shaderProgramID, "Colour_uniform");
		LightPositionUniformLocation = glGetUniformLocation(shaderProgramID, "LightPosition_uniform");
		AmbientUniformLocation = glGetUniformLocation(shaderProgramID, "Ambient_uniform");
		SpecularUniformLocation = glGetUniformLocation(shaderProgramID, "Specular_uniform");
		SpecularPowerUniformLocation = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");
	}
	/*	else if (togglelight == 1)
		{
			glUniform3f(ColourUniformLocation, 0.0, 0.0, 0.0);
			glUniform3f(LightPositionUniformLocation, 0, 0, 0);
			glUniform4f(AmbientUniformLocation, 0, 0, 0, 0.0);
			glUniform4f(SpecularUniformLocation, 0.0, 0.0, 0.0, 0.0);
			glUniform1f(SpecularPowerUniformLocation, 0);

		//	vertexNormalAttribute = 0;
			ColourUniformLocation = 0;
			LightPositionUniformLocation = 0;
			AmbientUniformLocation = 0;
			SpecularUniformLocation = 0;
			SpecularPowerUniformLocation = 0;
		} */
		else if (togglelight == 1) 
		{
			vertexNormalAttribute = glGetAttribLocation(shaderProgramID, "aVertexNormal");
			ColourUniformLocation = glGetUniformLocation(shaderProgramID, "Colour_uniform");
			LightPositionUniformLocation = glGetUniformLocation(shaderProgramID, "LightPosition_uniform");
			AmbientUniformLocation = glGetUniformLocation(shaderProgramID, "Ambient_uniform");
			SpecularUniformLocation = glGetUniformLocation(shaderProgramID, "Specular_uniform");
			SpecularPowerUniformLocation = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");
			vertexTexcoordAttribute = 0;
			TextureMapUniformLocation = 0;
		}


	//printf("togglelight = %i\n", togglelight);
	//printf("cockpit = %i\n", cockpitview);
	printf("score = %i\n", score);
//	printf("forward = %i\n", forward);
	printf("planemovex = %i\n", planemovex);
	//Unuse Shader
	glUseProgram(0);




	//Swap Buffers and post redisplay
	glutSwapBuffers();
	glutPostRedisplay();

	forward = 0;
	downpressed = 0;
	uppressed = 0;
}



//! Keyboard Interaction
void keyboard(unsigned char key, int x, int y)
{
	//Quits program when esc is pressed
	if (key == 27)	//esc key code
	{
		exit(0);
	}

	if (key == 40)	//esc key code
	{
		std::cout << "down arrow key pressed" << std::endl;
	}

	else if (key == 'w')
	{
		std::cout << "w key pressed" << std::endl;
	}


	//Set key status
	keyStates[key] = true;
}

//! Handle key up situation
void keyUp(unsigned char key, int x, int y)
{
	keyStates[key] = false;
}


//! Handle Keys
void handleKeys()
{
	//keys should be handled here
	if (keyStates['w'])
	{
		showmenu = 0;
		forward = 1;
	}

	if (keyStates['s'])
	{
		showmenu = 0;
		forward = -1;
	}

	if (((keyStates['a']) && speed > 0) || ((keyStates['a']) && forward < 0))
	{
		showmenu = 0;
		planerotation = planerotation+0.4;
	}

	if (((keyStates['d']) && speed > 0) || ((keyStates['d']) && forward < 0))
	{
		showmenu = 0;
		planerotation = planerotation - 0.4;
	}

	if ((keyStates['k']) && (speed > 0)) 
	{
		showmenu = 0;
		downpressed = 1;
		planeheightrotation = planeheightrotation - 0.25;
	}

	if ((keyStates['i']) && (speed > 0))
	{
		showmenu = 0;
		uppressed = 1;
		planeheightrotation = planeheightrotation + 0.25;
	}

	if ((keyStates['j']) && (speed > 0))
	{
		showmenu = 0;
		downpressed = 1;
		rollrotation = rollrotation - 0.4;
	}

	if ((keyStates['l']) && (speed > 0))
	{
		showmenu = 0;
		uppressed = 1;
		rollrotation = rollrotation + 0.4;
	}

	if (keyStates['o'])
	{
		showmenu = 1;
	}

	if (keyStates['r'])
	{
		reset = 1;
	}

	// Change level to 1

	if (keyStates['1'])
	{
		level = 1;
	}

	// Change level to 2
	
	if (keyStates['2'])
	{
		level = 2;
	}

	// Change level to 3

	if (keyStates['3'])
	{
		level = 3;
		randomrot = (rand() % 180);
	}

	if (keyStates['v'])
	{
		
			cockpitview = 1;
		

	}

	if (keyStates['b'])
	{
		cockpitview = 0;
	
	}

	if (keyStates['t'])
	{
		togglelight += 1;

		if (togglelight > 1)
		{
			togglelight = 0;
		}
	}

/*
	if (keyStates['t'])
	{
		togglefog += 1;

		if (togglefog > 1)
		{
			togglefog = 0;
		}
	} */
}

//! Mouse Interaction
void mouse(int button, int state, int x, int y)
{
	cameraManip.handleMouse(button, state, x, y);
	glutPostRedisplay();
}

//! Motion
void motion(int x, int y)
{
	cameraManip.handleMouseMotion(x, y);
	glutPostRedisplay();
}

//! Timer Function
void Timer(int value)
{
	// Do something

	//Call function again after 10 milli seconds
	glutTimerFunc(10, Timer, 0);
	t += 0.015;
	glutPostRedisplay();

}


void initShader()
{
	//Create shader
	shaderProgramID = Shader::LoadFromFile("shader.vert", "shader.frag");

	// Get a handle for our vertex position buffer
	vertexPositionAttribute = glGetAttribLocation(shaderProgramID, "aVertexPosition");
	vertexNormalAttribute = glGetAttribLocation(shaderProgramID, "aVertexNormal");

	//!
	MVMatrixUniformLocation = glGetUniformLocation(shaderProgramID, "MVMatrix_uniform");
	ProjectionUniformLocation = glGetUniformLocation(shaderProgramID, "ProjMatrix_uniform");
	ColourUniformLocation = glGetUniformLocation(shaderProgramID, "Colour_uniform");
	LightPositionUniformLocation = glGetUniformLocation(shaderProgramID, "LightPosition_uniform");
	AmbientUniformLocation = glGetUniformLocation(shaderProgramID, "Ambient_uniform");
	SpecularUniformLocation = glGetUniformLocation(shaderProgramID, "Specular_uniform");
	SpecularPowerUniformLocation = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");

	vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");
	TextureMapUniformLocation = glGetUniformLocation(shaderProgramID, "TextureMap_uniform");

	
}

float toRadian(float degree)
{
	return (degree*3.1415 / 180);
}

void render2dText(std::string text, float r, float g, float b,
	float x, float y)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(r, g, b);
	glRasterPos2f(x, y); // window coordinates
	for (unsigned int i = 0; i < text.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void handlelevel()
{
	//Loading level tutorial
	if (level == 1)
	{
		yposition = 2;
		//Init Map array from input.txt
		using namespace std;
		ifstream file("input1.txt");

		for (int j = 0; j < zlength; j++)
		{
			for (int i = 0; i < xlength; i++) file >> map[i][j];
		}
	}

	if (level == 2)
	{
		yposition = 3;
		//Init Map array from input.txt
		using namespace std;
		ifstream file("input2.txt");

		for (int j = 0; j < zlength; j++)
		{
			for (int i = 0; i < xlength; i++) file >> map[i][j];
		}
	}

	if (level == 3)
	{
		
		//Init Map array from input.txt
		using namespace std;
		ifstream file("input3.txt");

		for (int j = 0; j < zlength; j++)
		{
			for (int i = 0; i < xlength; i++) file >> map[i][j];
		}

		yposition = 3;
		//Init Map array from input.txt
		 randomx = (rand() % 38);
		 randomy = (rand() % 38);
		 randomz = (rand() % 38);
		 //randomrot = (rand() % 88);

		
	}
}
