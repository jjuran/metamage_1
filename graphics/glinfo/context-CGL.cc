/*
	context-CGL.cc
	--------------
*/

#ifdef __APPLE__

// Mac OS X
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>

// glinfo
#include "context.hh"


#ifndef NULL
#define NULL 0L
#endif


static CGLContextObj context;

int create_context()
{
	CGLError          err;
	GLint             nvirt;
	CGLPixelFormatObj format;
	
	const CGLPixelFormatAttribute attrs[] =
	{
		(CGLPixelFormatAttribute) NULL
	};
	
	err = CGLChoosePixelFormat( attrs, &format, &nvirt );
	
	if ( err )
	{
		return err;
	}
	
	err = CGLCreateContext( format, NULL, &context );
	
	CGLDestroyPixelFormat( format );
	
	if ( err )
	{
		return err;
	}
	
	CGLSetCurrentContext( context );
	
	return err;
}

void destroy_context()
{
	CGLDestroyContext( context );
	
	context = NULL;
}

#endif  // #ifdef __APPLE__
