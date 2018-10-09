#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutCreateWindow("I never get displayed");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
}