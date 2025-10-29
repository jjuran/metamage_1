/*
	blit_AGL.cc
	-----------
*/

#include "aglow/blit_AGL.hh"

// Mac OS X
#ifdef __APPLE__
#include <AGL/AGL.h>
#endif

// OpenGL
#include <OpenGL/gl.h>

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// glfb-common
#include "glfb/glfb.hh"


static AGLContext context;

void create_AGL_context()
{
	const GLint attrs[] =
	{
		AGL_RGBA,
		AGL_DOUBLEBUFFER,
		AGL_NONE  // "NULL" terminator
	};
	
	AGLPixelFormat format = aglChoosePixelFormat( NULL, 0, attrs );
	
	if ( format == NULL )
	{
		ExitToShell();
	}
	
	context = aglCreateContext( format, NULL );
	
	aglDestroyPixelFormat( format );
	
	aglSetCurrentContext( context );
	
	glfb::initialize();
}

void destroy_AGL_context()
{
	glfb::terminate();
	
	aglDestroyContext( context );
}

void attach_to_window( WindowRef window )
{
#ifdef MAC_OS_X_VERSION_10_5
	if ( &aglSetWindowRef != NULL )
	{
		aglSetWindowRef( context, window );
	}
	else
#endif
	{
		aglSetDrawable( context, GetWindowPort( window ) );
	}
	
	const Rect& portRect = mac::qd::get_portRect( window );
	
	glViewport( 0, 0, portRect.right, portRect.bottom );
}

void set_AGL_geometry( int stride, int width, int height, int depth )
{
	glfb::set_dimensions( width, height, depth );
}

void render_AGL()
{
	glfb::render();
	
	glFlush();
	aglSwapBuffers( context );
}
