/*
	blit_AGL.hh
	-----------
*/

#ifndef AGLOW_BLITAGL_HH
#define AGLOW_BLITAGL_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


void create_AGL_context();
void destroy_AGL_context();

void attach_to_window( WindowRef window );

void set_AGL_geometry( int stride, int width, int height );

void render_AGL();

#endif
