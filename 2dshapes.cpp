/*
 * 2dshapes.cpp
 *
 *  Created on: Mar 15, 2019
 *      Author: a_dad_000
 */
#include<GL/freeglut.h> // Include the freeGLUT header file

/* Implements Display Callback Handler*/
void displayGraphics(){

	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to
	Black with full Opacity.*/
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);  // Uses the color buffer and sets the background color of the window
	glBegin(GL_POLYGON);   //Starting delimiter for Star primitive
		glColor3f(1.0f, 0.0f, 0.0f); //Sets the vertex color to Red using floating point values

		/* Sets 10 vertices to form the Star*/
		glVertex2f(0.1f,   0.2f);
		glVertex2f(0.3f,   0.2f);
		glVertex2f(0.15f,   0.0f);
		glVertex2f(0.2f,   -0.2f);
		glVertex2f(0.0f,   -0.1f);
		glVertex2f(-0.2f,   -0.2f);
		glVertex2f(-0.15f,   0.0f);
		glVertex2f(-0.3f,   0.2f);
		glVertex2f(-0.1f,   0.2f);
		glVertex2f(0.0f,   0.4f);




	glEnd(); //Ending delimiter for Star primitive

	glBegin(GL_POLYGON);   //Starting delimiter for Pentagon primitive
		glColor3f(0.0f, 0.0f, 1.0f); //Sets the vertex color to Blue using floating point values

		/* Sets 5 vertices to form the Pentagon*/
        glVertex2f(-0.6f,   -0.2f);
        glVertex2f(-0.35f,   -0.36f);
        glVertex2f(-0.4f,   -0.65f);
        glVertex2f(-0.8f,   -0.65f);
        glVertex2f(-0.85f, -0.36f);


	glEnd(); //Ending delimiter for Pentagon primitive

	glBegin(GL_POLYGON);   //Starting delimiter for Heptagon primitive
		glColor3f(0.0f, 1.0f, 0.0f); //Sets the vertex color to Green using floating point values

		/* Sets 7 vertices to form the Heptagon*/
        glVertex2f(0.65f,-0.11f);
        glVertex2f(0.85f,   -0.22f);
        glVertex2f(0.95f,   -0.42f);
        glVertex2f(0.80f,   -0.67f);
        glVertex2f(0.50f,   -0.67f);
        glVertex2f(0.35f, -0.42f);
        glVertex2f(0.45f, -0.22f);

	glEnd(); //Ending delimiter for Heptagon primitive


	glFlush(); // Empties all buffers and executes all issued commands to be accepted by the rendering engine

}

/* Main function required for Immediate mode */
int main (int argc, char** argv){
	glutInit(&argc, argv);  // Initializes the freeGLUT library
	glutCreateWindow("OpenGL 2dshapes"); // Create a window and title
	glutInitWindowSize(1280,720); // Specifies the window's width and height
	glutInitWindowPosition(0, 0); // Specifies the position of the window's top-left corner
	glutDisplayFunc(displayGraphics); // Sets the display callback for the current window
	glutMainLoop(); // Enters the GLUT event processing loop
	return 0;  // Exits main function

}







