/*
 * ModFour.cpp
 *
 *  Created on: Mar 30, 2019
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

#define WINDOW_TITLE "Assignment 4-1-4" // Macro for window title

//Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/*Variable declaration for shader, window size initialization , buffer and array objects*/
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, EBO, texture;


/*Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);



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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color
	glutDisplayFunc(URenderGraphics);
	glutMainLoop();

	//Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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

	//Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Place the object at the center of the viewport
	model = glm::rotate(model, 120.0f, glm::vec3(1.0f, 1.0f, 1.0f)); //Rotate the object 45 degrees on the XYZ
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increase the object size by a scale of 2

	//Transforms the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.5f, 0.0f, -5.0f)); //moves the camera backwards -5 units in Z

	//Creates an othographic projection
	glm::mat4 projection;
	projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	//Draws the triangles
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
			//Vertex Positions //Colors
			0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, //Vertex top middle 0
			0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom Right Vertex 1
			-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, //Bottom Left Vertex 2
			//-0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 1.0f, //Top Left Vertex 3

			0.5f, -0.5f, -1.0f,  0.5f, 0.5f, 1.0f, //3 br right
			//0.5f, 0.5f, -1.0f, 1.0f, 1.0f, 0.5f, // 5 tl right
			//-0.5f, 0.5f, -1.0f, 0.2f, 0.2f, 0.5f, //6 tl top
			-0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 1.0f //4 bl back
				};
	//Index data to share position data
	GLuint indices[] = {
			0, 1, 2, //Triangle 1
			0, 2, 4,  //Triangle 2
			0, 4, 3,  //Triangle 3
			1, 2, 3,  //Triangle 4
			2, 4, 3,  //Triangle 5


	};


	//Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers.
	glBindVertexArray(VAO);

	//Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copy vertices to BVO

	//Activate the Element Buffer Object /Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copy indices to EBO

	//Set attribute pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); //Enables vertex attribute

	//Set attribute pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);  //Enables vertex attribute

	glBindVertexArray(0); //Deactivates the VAO which is good practice

}






