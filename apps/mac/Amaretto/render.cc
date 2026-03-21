/*
	render.cc
	---------
*/

#include "render.hh"

// OpenGL
#include <OpenGL/gl.h>

// glfb-common
#include "glfb/glfb.hh"


void render()
{
	glfb::render();
	
	glFlush();
}
