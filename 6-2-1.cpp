/*
 * Tutorial6-2.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: Azita Dadresan
 */
/*Header Inclusions*/
#include <iostream> // Includes C++ i/o stream
#include <GL/glew.h>  //Includes glew header
#include <GL/freeglut.h> //Includes freeglut header
#include "SOIL2/SOIL2.h" //SOIL Image Loader Inclusion

//GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; //Uses the standard namespace

#define WINDOW_TITLE "Modern OpenGL" // Macro for window title

//Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/*Variable declaration for shader, window size initialization, buffer and array objects*/
GLint cubeShaderProgram, lampShaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, CubeVAO, LightVAO, texture;
GLfloat degrees = glm::radians(-45.0f); //Converts float to degrees

//Subject position and scale
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
glm::vec3 cubeScale(2.0f);

//Cube and light color
glm::vec3 objectColor(0.6f, 0.5f, 0.75f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

//Light position and scale
glm::vec3 lightPosition(0.5f, 0.5f, -3.0f);
glm::vec3 lightScale(0.3f);

//Camera position
glm::vec3 cameraPosition(0.0f, 0.0f, -6.0f);

//Camera rotation
float cameraRotation = glm::radians(-25.0f);

/*Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);


/*Vertex Shader Program Source Code*/
const GLchar *cubeVertexShaderSource = GLSL(330,
		layout(location=0) in vec3 position;//Vertex data from vertex attrib pointer 0
		layout(location=1) in vec3 normal;  //VAP position 1 for normals
		layout(location=2) in vec2 textureCoordinates;  //Vertex texture

		out vec3 Normal;  //For outgoing normals to fragment shader
		out vec3 FragmentPos;  //For outgoing color / pixels to fragment shader
		out vec2 mobileTextureCoordinate; //For outgoing texture

		//Uniform /Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;


		 void main() {
		  gl_Position = projection * view * model * vec4(position, 1.0f); //Transform vertices to clip coordinates
		  FragmentPos = vec3(model * vec4(position, 1.0f));  //Gets fragment / pixel position in world space only (exclude view and projection)
		  Normal = mat3(transpose(inverse(model)))*normal; //get normal vectors in world space only and exclude normal translation properties
		  mobileTextureCoordinate = vec2(textureCoordinates.x, 1.0f - textureCoordinates.y); //flips the texture horizontally
		 }

);

/*Fragment Shader Program Source Code*/
const GLchar *cubeFragmentShaderSource = GLSL(330,

		in vec3 Normal;  //For incoming normals
		in vec3 FragmentPos; // For incoming fragment position
		in vec2 mobileTextureCoordinate; //For incoming Texture coordinate

		out vec4 cubeColor;  //For outgoing cube color to the GPU
		out vec4 gpuTexture;  //Variable to pass texture data to the GPU
		uniform sampler2D uTexture; //Useful when working with multiple textures

		//Uniform / Global variables for object color, light color, light position, and camera/view position
		uniform vec3 objectColor;
		uniform vec3 lightColor;
		uniform vec3 lightPos;
		uniform vec3 viewPosition;


		void main(){


			gpuTexture = texture(uTexture, mobileTextureCoordinate); //Sends texture to the GPU for rendering


			/*Phong lighting model calculations to generate ambient, diffuse, and specular components */
			//Calculate Ambient lighting for light source 1
			float ambientStrength = 0.0f; //Set ambient or global lighting strength
			vec3 ambient = ambientStrenght * lightColor; //Generate ambient light color

			//Calculate Diffuse lighting for light source 1
			vec3 norm = normalize(Normal); //Normalize vectors to 1 unit
			vec3 lightDirection = normalize(lightPos); //Calculate distance (light direction)between light source and fragment/pixels on
			float impact = max(dot(norm, lightDirection), 0.0); //Calculate diffues impact by generating dot product of normal and light
			vec3 diffuse = impact * lightColor; //Generate diffuse light color

			//Calculate Specular lighting for light source 1
			float specularIntensity = 0.1f; //Set specular light strength
			float highLightSize = 128.0f; // Set specular highlight size
			vec3 viewDir = normalize(viewPosition - FragmentPos); //Calculate view direction
			vec3 reflectDir = reflect(-lightDirection, norm); //Calculate reflection vector

			//Calculate specular component for light source 1
			float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
			vec3 specular = specularIntensity * specularComponent * lightColor;

			//Calculate phong result
			vec3 phong = (ambient + diffuse + specular) * objectColor;
			cubeColor = vec4(phong, 1.0f); //Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar *lampVertexShaderSource = GLSL(330,

		layout (location = 0) in vec3 position; //VAP position 0 for vertex position data

		//Uniform /Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;


		 void main() {
		  gl_Position = projection * view * model * vec4(position, 1.0f); //Transform vertices to clip coordinates

		 }


);


/* Fragment Shader Source Code*/
const GLchar * lampFragmentShaderSource = GLSL(330,

		out vec4 color; //For outgoing lamp color(smaller cube) to the GPU

		void main()
		{

			color = vec4(1.0f); // Set color to white (1.0f, 1.0f, 1.0f) with alpha 1.0
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
	UGenerateTexture();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color

	glutDisplayFunc(URenderGraphics);


	glutMainLoop();

	//Destroys Buffer objects once used
	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteVertexArrays(1, &LightVAO);
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
	glEnable(GL_DEPTH_TEST); //Enable Z-depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	GLint modelLoc, viewLoc, projLoc, objectColorLoc, lightColorLoc, lightPositionLoc, viewPositionLoc;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	//Use the cube Shader and activate the cube vertex array object for rendering and transorming
	glUseProgram(cubeShaderProgram);
	glBindVertexArray(CubeVAO);

	//Transforms the cube
	model = glm::translate(model, cubePosition); // Place the object at the center of the viewport
	model = glm::scale(model, cubeScale); // Increase the object size by a scale of 2

	//Transforms the camera
	view = glm::translate(view, cameraPosition);
	view = glm::rotate(view, cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f));

	//Creates a perspective projection
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	//Retrieves and passes transform matrices to the cube shader program
	modelLoc = glGetUniformLocation(cubeShaderProgram, "model");
	viewLoc = glGetUniformLocation(cubeShaderProgram, "view");
	projLoc = glGetUniformLocation(cubeShaderProgram, "projection");

	//Pass matrix data to the Cube shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//Reference matrix uniforms from the cube shader program for the cube color, light color, light position and camera position
	objectColorLoc = glGetUniformLocation(cubeShaderProgram, "objectColor");
	lightColorLoc = glGetUniformLocation(cubeShaderProgram, "lightColor");
	lightPositionLoc = glGetUniformLocation(cubeShaderProgram, "lightPos");
	viewPositionLoc = glGetUniformLocation(cubeShaderProgram, "viewPosition");

	//Pass color, light, and camera data to the cube shader program's corresponding uniforms
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


	glutPostRedisplay();
	glBindTexture(GL_TEXTURE_2D, texture);



	//Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0); //Decativate the Vertex Array Object



	//Use the light Shader and activate the lamp vertex array object for rendering and transorming
		glUseProgram(lampShaderProgram);
		glBindVertexArray(LightVAO);

		//Transforms the smaller cube used as a visual que for the light source
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, lightScale);

		//Retrieves and passes transform matrices to the lamp shader program
		modelLoc = glGetUniformLocation(lampShaderProgram, "model");
		viewLoc = glGetUniformLocation(lampShaderProgram, "view");
		projLoc = glGetUniformLocation(lampShaderProgram, "projection");

		//Pass matrix data to the Cube shader program's matrix uniforms
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));




		//Draws the triangles
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0); //Decativate the Vertex Array Object
		glutPostRedisplay(); //marks the current window to be redisplayed

		glutSwapBuffers(); //Flips the back buffer with the front buffer every frame. Similar to GL Flush

}


/*Creates the Shader program*/
void UCreateShader()
{
	//Cube Vertex Shader
	GLint cubeVertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the vertex Shader
	glShaderSource(cubeVertexShader, 1, &cubeVertexShaderSource, NULL); //Attaches the vertex shader to the source code
	glCompileShader(cubeVertexShader); //Compiles the Vertex shader

	//Cube Fragment Shader
	GLint cubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the Fragment shader
	glShaderSource(cubeFragmentShader, 1, &cubeFragmentShaderSource, NULL); //Attaches the Fragment Shader to the source code
	glCompileShader(cubeFragmentShader); //Compiles the Fragment Shader

	//Cube Shader program
	cubeShaderProgram = glCreateProgram(); //Creates the shader program and returns an id
	glAttachShader(cubeShaderProgram, cubeVertexShader); //Attach vertex shader to the shader program
	glAttachShader(cubeShaderProgram, cubeFragmentShader); //Attach Fragment shader to the shader program
	glLinkProgram(cubeShaderProgram); //Link Vertex and Fragment shaders to shader program

	//Delete the Cube vertex and Cube fragment shaders once linked
	glDeleteShader(cubeVertexShader);
	glDeleteShader(cubeFragmentShader);

	//Lamp Vertex Shader
	GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the vertex Shader
	glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL); //Attaches the vertex shader to the source code
	glCompileShader(lampVertexShader); //Compiles the Vertex shader

	//Lamp Fragment Shader
	GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the Fragment shader
	glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL); //Attaches the Fragment Shader to the source code
	glCompileShader(lampFragmentShader); //Compiles the Fragment Shader

	//Lamp Shader program
	lampShaderProgram = glCreateProgram(); //Creates the lamp shader program and returns an id
	glAttachShader(lampShaderProgram, lampVertexShader); //Attach lamp vertex shader to the shader program
	glAttachShader(lampShaderProgram, lampFragmentShader); //Attach lamp Fragment shader to the shader program
	glLinkProgram(lampShaderProgram); //Link lamp Vertex and lamp Fragment shaders to shader program

	//Delete the Lamp vertex shaders once linked
	glDeleteShader(lampVertexShader);
	glDeleteShader(lampFragmentShader);




}

/*Creates the Buffer and Array Objects*/
void UCreateBuffers()
{
	//Position and Normals data
	GLfloat vertices[] = {

			//Positions					//Normals
			//back face					//negative z normals   //Texture Coordinates
			0.0f, 0.0f, 0.0f, 	    0.0f,  0.0f, -1.0f,         0.0f,  0.0f,
			0.5f, 1.0f, 0.5f, 	     0.0f,  0.0f, -1.0f,		0.5f,  1.0f,
			1.0f,  0.0f, 0.0f,    	 0.0f,  0.0f, -1.0f,		1.0f,  1.0f,

			//Right Face				//Positive x normals
			 1.0f, 0.0f, 0.0f, 	  	1.0f,  0.0f, 0.0f,			1.0f,  1.0f,
			 0.5f, 1.0f, 0.5f,	 	1.0f,  0.0f, 0.0f,			0.5f,  1.0f,
			 1.0f, 0.0f, 1.0f,	  	1.0f,  0.0f, 0.0f,			0.0f,  0.0f,

			 //Front Face			//Positive Z normals
			1.0f,  0.0f,  1.0f, 	  0.0f,  0.0f, 1.0f,		0.0f,  0.0f,
			0.5f, 1.0f, 0.5f,	 	  0.0f,  0.0f, 1.0f,		0.5f,  1.0f,
			0.0f, 0.0f, 1.0f,	  	  0.0f,  0.0f, 1.0f,		1.0f,  1.0f,

			// Left Face			//Negative x normals
			 0.0f,  0.0f,  1.0f, 	  	-1.0f,  0.0f, 0.0f,		1.0f,  1.0f,
			 0.5f, 1.0f, 0.5f,	 	    -1.0f,  0.0f, 0.0f,		0.5f,  1.0f,
			 0.0f,  0.0f,  0.0f,	  	-1.0f,  0.0f, 0.0f,		0.0f,  0.0f,

			 // Bottom Face			//Nnegative Y normals
			 0.0f,  0.0f, 0.0f, 	  	0.0f, -1.0f, 0.0f,		0.0f,  0.0f,
			 1.0f,  0.0f,  0.0f,	 	0.0f, -1.0f, 0.0f,		0.5f,  1.0f,
			 1.0f,  0.0f,  1.0f,	  	0.0f, -1.0f, 0.0f,		1.0f,  1.0f,

			 1.0f,  0.0f, 1.0f, 	  	0.0f, -1.0f, 0.0f,		1.0f,  1.0f,
			 0.0f,  0.0f,  1.0f,	 	0.0f, -1.0f, 0.0f,		0.5f,  1.0f,
			 0.0f,  0.0f,  0.0f,	  	0.0f, -1.0f, 0.0f,		0.0f,  0.0f

			/*//Vertex Positions	 	   //Normal 				//Texture Coordinate
			-0.5f, -0.5f, -0.5f, 	  	0.0f,  0.0f,  -1.0f,	    0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,	 	0.0f,  0.0f,  -1.0f,	    1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,	  	0.0f,  0.0f,  -1.0f,		1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f, 		0.0f,  0.0f,  -1.0f,		1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f, 	    0.0f,  0.0f,  -1.0f,		0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, 	    0.0f,  0.0f,  -1.0f,		0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f, 	  	0.0f,  0.0f,  1.0f,			0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,	 	0.0f,  0.0f,  1.0f,			1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,	  	0.0f,  0.0f,  1.0f,			1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f, 		0.0f,  0.0f,  1.0f,			1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f, 	    0.0f,  0.0f,  1.0f,			0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 	    0.0f,  0.0f,  1.0f,			0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, 	  	-1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,	 	-1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,	  	-1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, 		-1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 	    -1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 	    -1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f, 	  	1.0f,  0.0f,  0.0f,			1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,	 	1.0f,  0.0f,  0.0f,			1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,	  	1.0f,  0.0f,  0.0f,			0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f, 		1.0f,  0.0f,  0.0f,			0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f, 	    1.0f,  0.0f,  0.0f,			0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f, 	    1.0f,  0.0f,  0.0f,			1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f, 	  	0.0f,  -1.0f,  0.0f,		0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,	 	0.0f,  -1.0f,  0.0f,		1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,	  	0.0f,  -1.0f,  0.0f,		1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f, 		0.0f,  -1.0f,  0.0f,		1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, 	    0.0f,  -1.0f,  0.0f,		0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 	    0.0f,  -1.0f,  0.0f,		0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f, 	  	0.0f,  1.0f,  0.0f,			0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,	 	0.0f,  1.0f,  0.0f,			1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,	  	0.0f,  1.0f,  0.0f,			1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f, 		0.0f,  1.0f,  0.0f,			1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 	    0.0f,  1.0f,  0.0f,			0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, 	    0.0f,  1.0f,  0.0f,			0.0f,  1.0f*/








	};



	//Generate buffer ids
	glGenVertexArrays(1, &CubeVAO); //Vertex Array object for cube vertices
	glGenBuffers(1, &VBO);



	// Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers.
	glBindVertexArray(CubeVAO);

	//Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copy vertices to VBO


	//Set attribute pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); //Enables vertex attribute


	//Set attribute pointer 2 to hold Normal Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);  //Enables vertex attribute

	//Set attribute pointer 2 to hold Texture coordinate Data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);  //Enables vertex attribute

	glBindVertexArray(0); //Deactivates the VAO which is good practice

	//Generate buffer ids for lamp(smaller cube)
	glGenVertexArrays(1, &LightVAO); //vertex array object for cube vertex copies to serve as light source

	//Activate the vertex array object before binding and setting any VBOs and vertex attribute pointers
	glBindVertexArray(LightVAO);

	//Regerencing the same VBO for its vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Set attribute pointer  to hold poistion data (used for the lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);



}

void UGenerateTexture(){
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;

	unsigned char* image = SOIL_load_image("snhu.jpg", &width, &height, 0, SOIL_LOAD_RGB); //Loads texture file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); //Unbind the texture

}







