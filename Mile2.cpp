/*
 * ModFour.cpp
 *
 *  Created on: Apr 6, 2019
 *      Author: Azita Dadresan
 */

/*Header Inclusions*/
#include <iostream> // Includes C++ i/o stream
#include <GL/glew.h>  //Includes glew header
#include <GL/freeglut.h> //Includes freeglut header

//GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; //Uses the standard namespace

#define WINDOW_TITLE "Final Project Milestone 2" // Macro for window title

//Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/*Variable declaration for shader, window size initialization, buffer and array objects*/
GLint shaderProgram, WindowWidth = 1024, WindowHeight = 1024;
GLuint VBO, VAO;

GLfloat cameraSpeed = 0.0005f; //Movement speed per frame

GLchar currentKey; //Will store key pressed

GLfloat lastMouseX = 400, lastMouseY = 300; //Locks mouse cursor at the ecnter of the screen
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; //mouse offset, yaw, and pitch variables
GLfloat sensitivity = 0.005f; //Used for mouse / camera rotation sensitivity
bool mouseDetected = true; //Initially true when mouse movement is detected


//Global vector declarations
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f); //Initial camera position.
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); //Temporary y unit vector
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); //Temporary z unit vector
glm::vec3 front; //Temporary z unit vector for mouse

/*Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UKeyboard(unsigned char key, int x, int y);
void UKeyReleased(unsigned char key, int x, int y);
void UMouseClick(int button, int state, int x, int y);
void UMouseMove(int x, int y);
void UMousePressedMove(int x, int y);



/*Vertex Shader Program Source Code*/
const GLchar *vertexShaderSource = GLSL(330,
		layout(location=0) in vec3 position;//Vertex data from vertex attrib pointer 0
		layout(location=1) in vec3 color;//Color data from vertex attrib pointer 1

		out vec3 mobileColor; //variable to transfer color data to the fragment shader

		//Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;


		 void main() {
		  gl_Position = projection * view * model * vec4(position, 1.0f); //Transform vertices to clip coordinates
		  mobileColor = color;// references incoming color data
		 }

);

/*Fragment Shader Program Source Code*/
const GLchar *fragmentShaderSource = GLSL(330,
		in vec3 mobileColor; //Variable to hold incoming color data from vertex shader
		out vec4 gpuColor; //Variable to pass color data to the GPU

		void main(){

			gpuColor = vec4(mobileColor, 1.0); //Sends color data to the GPU for rendering
}
);


/*Main Program*/
int main(int argc, char* argv[])
{
	glutInit(&argc, argv); //Initialize the OpenGL program
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);


	glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
			{
				std::cout << "Failed to initialize GLEW" << std::endl;
				return -1;
			}


	UCreateShader();
	UCreateBuffers();

	// Use the Shader program
	glUseProgram(shaderProgram);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //Set background color
	glutDisplayFunc(URenderGraphics);
	glutKeyboardFunc(UKeyboard); //Detects key press
	glutKeyboardUpFunc(UKeyReleased); //Detects key release
	glutMouseFunc(UMouseClick); //Detects mouse click

	glutMotionFunc(UMousePressedMove); //Detects mouse press and movement


	glutPassiveMotionFunc(UMouseMove); //Detects mouse movement

	glutMainLoop();

	//Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;

}

/*Resizes the window*/
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);

}

/*Renders Graphics*/
void URenderGraphics(void)
{
	glEnable(GL_DEPTH_TEST); //Enable z-depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glBindVertexArray(VAO); //Activate the Vertex Array Object before rendering and transforming them

	/*Camera Movement Logic*/

		if(currentKey == 'w')
			cameraPosition += cameraSpeed * CameraForwardZ;

		if(currentKey == 's')
			cameraPosition -= cameraSpeed * CameraForwardZ;

		if(currentKey == 'a')
			cameraPosition -= glm::normalize(glm::cross(CameraForwardZ, CameraUpY))* cameraSpeed;

		if(currentKey == 'd')
			cameraPosition += glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed;

		CameraForwardZ = front; //Replaces camera forward vector with Radians normalized as a unit vector

	//Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Place the object at the center of the viewport
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Rotate the object 45 degrees on the XYZ
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increase the object size by a scale of 2

	//Transforms the camera
	glm::mat4 view;
	view = glm::lookAt(CameraForwardZ, cameraPosition, CameraUpY);

	//Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	//Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 180);

	glBindVertexArray(0); //Decativate the Vertex Array Object

	glutSwapBuffers(); //Flips the back buffer with the front buffer every frame. Similar to GL Flush

}


/*Creates the Shader program*/
void UCreateShader()
{
	//Vertex Shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the vertex Shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //Attaches the vertex shader to the source code
	glCompileShader(vertexShader); //Compiles the Vertex shader

	//Fragment Shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the Fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); //Attaches the Fragment Shader to the source code
	glCompileShader(fragmentShader); //Compiles the Fragment Shader

	//Shader program
	shaderProgram = glCreateProgram(); //Creates the shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); //Attach vertex shader to the shader program
	glAttachShader(shaderProgram, fragmentShader); //Attach Fragment shader to the shader program
	glLinkProgram(shaderProgram); //Link Vertex and Fragment shaders to shader program

	//Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

/*Creates the Buffer and Array Objects*/
void UCreateBuffers()
{
	//Position and color data
	GLfloat vertices[] = {
			//Vertex Positions	 	   //Black Color
			-1.0f, -0.05f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f, 		0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			-1.0f, -0.05f,  0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f,  0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.5f, 		0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,

			-1.0f,  0.05f,  0.5f, 	  	0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f, -0.5f, 		0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,

			 1.0f,  0.05f,  0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f, -0.5f, 		0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,

			-1.0f, -0.05f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f,  0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -0.05f,  0.5f, 		0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,
			-1.0f, -0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			-1.0f,  0.05f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.5f, 		0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f,  0.5f, 	    0.0f,  0.0f,  0.0f,
			-1.0f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			//Leg one	 	            //Black Color

			 -0.8f, -2.0f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, -0.5f, 		0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			-0.8f, -2.0f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  -0.4f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			-0.8f,  0.05f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f, -0.5f,	  	    0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f, -0.5f, 		0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			 -1.0f,  0.05f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, -0.5f, 		0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			 -0.8f, -2.0f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			-0.8f,  0.05f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,


			  //Leg two  	            //Black Color

			 0.8f, -2.0f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f, 		0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			0.8f, -2.0f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  -0.4f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			0.8f,  0.05f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			0.8f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			0.8f, -2.0f, -0.5f,	  	    0.0f,  0.0f,  0.0f,
			0.8f, -2.0f, -0.5f, 		0.0f,  0.0f,  0.0f,
			0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			 1.0f,  0.05f,  -0.4f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, -0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, -0.5f, 		0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,

			 0.8f, -2.0f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f, -0.5f, 	    0.0f,  0.0f,  0.0f,

			 0.8f,  0.05f, -0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, -0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  -0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  -0.4f, 		0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f,  -0.4f, 	    0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f, -0.5f, 	    0.0f,  0.0f,  0.0f,


			 //Leg Three	 	            //Black Color

			 0.8f, -2.0f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, 0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, 0.5f, 		0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f, 0.5f, 	    0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f, 0.5f, 	    0.0f,  0.0f,  0.0f,

			0.8f, -2.0f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  0.4f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.4f, 		0.0f,  0.0f,  0.0f,
			0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			0.8f,  0.05f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			0.8f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			0.8f, -2.0f, 0.5f,	  	    0.0f,  0.0f,  0.0f,
			0.8f, -2.0f, 0.5f, 		0.0f,  0.0f,  0.0f,
			0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			 1.0f,  0.05f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, 0.5f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, 0.5f, 		0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			 0.8f, -2.0f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f, -2.0f,  0.4f, 		0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 0.8f, -2.0f, 0.5f, 	    0.0f,  0.0f,  0.0f,

			 0.8f,  0.05f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 1.0f,  0.05f,  0.4f, 		0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 0.8f,  0.05f, 0.5f, 	    0.0f,  0.0f,  0.0f,


			 //Leg Four	 	            //Black Color
			 -0.8f, -2.0f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, 0.5f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, 0.5f, 		0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f, 0.5f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f, 0.5f, 	    0.0f,  0.0f,  0.0f,

			-0.8f, -2.0f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  0.4f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  0.4f, 		0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			-0.8f,  0.05f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f, 0.5f,	  	    0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f, 0.5f, 		0.0f,  0.0f,  0.0f,
			-0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			-0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			 -1.0f,  0.05f,  0.4f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, 0.5f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, 0.5f, 		0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,

			 -0.8f, -2.0f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f, -2.0f,  0.4f, 		0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f, -2.0f, 0.5f, 	    0.0f,  0.0f,  0.0f,

			 -0.8f,  0.05f, 0.5f, 	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f, 0.5f,	 	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  0.4f,	  	0.0f,  0.0f,  0.0f,
			 -1.0f,  0.05f,  0.4f, 		0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f,  0.4f, 	    0.0f,  0.0f,  0.0f,
			 -0.8f,  0.05f, 0.5f, 	    0.0f,  0.0f,  0.0f









	};


	//Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);


	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers.
	glBindVertexArray(VAO);

	//Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copy vertices to BVO

	//Set attribute pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); //Enables vertex attribute

	//Set attribute pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);  //Enables vertex attribute

	glBindVertexArray(0); //Deactivates the VAO which is good practice

}

/*Implements the UKeyboard function*/
void UKeyboard(unsigned char key, GLint x, GLint y)
{

	switch(key){

		case 'w':
			currentKey = key;
			cout<< "You pressed W!" <<endl;
			break;

		case 's':
			currentKey = key;
			cout<< "You pressed S!" <<endl;
			break;

		case 'a':
			currentKey = key;
			cout<< "You pressed A!" <<endl;
			break;

		case'd':
			currentKey = key;
			cout<<"You pressed D!" <<endl;
			break;

		default:
			currentKey = key;
			cout<<"Press a key!" <<endl;


	}

}

/*Implements the UKeyReleased function*/
void UKeyReleased(unsigned char key, GLint x, GLint y)
{

	cout<<"Key Released!"<<endl;
	currentKey = '0';

}

/*Implements the UMouseMove function*/
void UMouseMove(int x, int y)
{
	//Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected)
	{
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	//Gets the direction the mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y; //Inverted Y

	//Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	//Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	//Accumulates the yaw and pitch variables
	yaw += mouseXOffset;
	pitch += mouseYOffset;


	//Orbits around the center
	front.x = 10.0f * cos(yaw);
	front.y = 10.0f * sin(pitch);
	front.z = sin(yaw) * cos(pitch) * 10.0f;


}


/*Implements the UMouseClick function*/
void UMouseClick(int button, int state, int x, int y)
{
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
		cout << "Left Mouse Button Clicked!" <<endl;
	}

	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)){
		cout << "Left Mouse Button Released!" <<endl;
	}
}



/* Implements the UMousePressedMove function*/
void UMousePressedMove(int x, int y)
{
	cout<< "Mouse pressed and moved!" <<endl;
}



















