/*
	blit_CGL.cc
	-----------
*/

#include "blit_CGL.hh"

// Mac OS X
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

// OpenGL
#include <OpenGL/gl.h>

// glfb-common
#include "glfb/glfb.hh"


static CGLContextObj context;

CGL_blitter::CGL_blitter( CGDirectDisplayID id )
{
	CGLError err;
	
	CGOpenGLDisplayMask display_mask = CGDisplayIDToOpenGLDisplayMask( id );
	
	const CGLPixelFormatAttribute attrs[] =
	{
	#ifndef MAC_OS_X_VERSION_10_7
		// Without this, CGLSetFullScreen() returns 10004 in 10.6.
		kCGLPFAFullScreen,
	#endif
		kCGLPFADoubleBuffer,
		kCGLPFADisplayMask,
		(CGLPixelFormatAttribute) display_mask,
		(CGLPixelFormatAttribute) 0  // "NULL" terminator
	};
	
	CGLPixelFormatObj format;
	GLint             nvirt;
	
	if ( (err = CGLChoosePixelFormat( attrs, &format, &nvirt )) )
	{
		return;
	}
	
	err = CGLCreateContext( format, 0, &context );  // NULL
	
	CGLDestroyPixelFormat( format );
	
	if ( err )
	{
		return;
	}
	
#ifdef MAC_OS_X_VERSION_10_7
	
	/*
		According to the Internet, 10.5 lacks CGLSetFullScreenOnDisplay(),
		even though Apple's headers claim otherwise.
		
		In addition, CGLSetFullScreenOnDisplay() is returning 10012 in 10.6.
	*/
	
	err = CGLSetFullScreenOnDisplay( context, display_mask );
	
#else
	
	err = CGLSetFullScreen( context );
	
#endif
	
	if ( err )
	{
		return;
	}
	
	CGLSetCurrentContext( context );
	
	glfb::initialize();
}

CGL_blitter::~CGL_blitter()
{
	// Clear and flush twice to ensure we get a black screen before exit.
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	glfb::terminate();
	
	CGLClearDrawable( context );
	CGLDestroyContext( context );
}

void CGL_blitter::prep( int stride, int width, int height )
{
	glfb::set_dimensions( width, height );
}

void CGL_blitter::area( CGRect bounds )
{
	glViewport( (int) bounds.origin.x,
	            (int) bounds.origin.y,
	            (int) bounds.size.width,
	            (int) bounds.size.height );
}

void CGL_blitter::blit( const void* src_addr )
{
	glfb::blit( src_addr );
	
	CGLFlushDrawable( context );  // implicitly calls glFlush()
}
