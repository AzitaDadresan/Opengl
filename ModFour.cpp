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

#define WINDOW_TITLE "Assignment 4-1" // Macro for window title

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

		uniform mat4 shaderTransform; //4x4 matrix variable for tranforming vertex data

		void main(){
			gl_Position = shaderTransform * vec4(position, 1.0f); //transform vertex data using matrix
			mobilecolor = color; // references incoming color data
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glBindVertexArray(VAO); //Activate the Vertex Array Object before rendering and transforming them

	//Declares a 4x4 identity matrix uniform variable to handle transformations
	glm::mat4 currentTransform;

	//Moves the 0.5 in y
	currentTransform = glm::translate(currentTransform, glm::vec3(0.0f, 0.5f, 0.0f));

	//Rotates shape 45 degrees on the z axis
	currentTransform = glm::rotate(currentTransform, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	//Scales the shape down by half it's original size in xyz
	currentTransform = glm::scale(currentTransform, glm::vec3(0.5f, 0.5f, 0.5f));

	//Sends transform information to the Vertex shader
	GLuint transformLocation = glGetUniformLocation(shaderProgram, "ShaderTransform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(currentTransform));

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
			0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, //Top Right Vertex 0
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom Right Vertex 1
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, //Bottom Left Vertex 2
			-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f //Top Left Vertex 3
				};
	//Index data to share position data
	GLuint indices[] = {
			0, 1, 3, //Trinagle 1
			1, 2, 3  //Triangle 2
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

