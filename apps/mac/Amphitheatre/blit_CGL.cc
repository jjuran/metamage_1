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

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"


int image_width;
int image_height;

int tex_width;
int tex_height;

const GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB;

int tex_x1;
int tex_y1;

static uint8_t* texture_data;

static CGLContextObj context;
static GLuint        texture;

CGL_blitter::CGL_blitter( CGDirectDisplayID id, CGRect bounds )
{
	CGLError err;
	
	CGOpenGLDisplayMask display_mask = CGDisplayIDToOpenGLDisplayMask( id );
	
	const CGLPixelFormatAttribute attrs[] =
	{
	#ifndef MAC_OS_X_VERSION_10_6
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
	
#ifdef MAC_OS_X_VERSION_10_6
	
	/*
		According to the Internet, 10.5 lacks CGLSetFullScreenOnDisplay(),
		even though Apple's headers claim otherwise.
	*/
	
	err = CGLSetFullScreenOnDisplay( context, display_mask );
	
#else
	
	err = CGLSetFullScreen( context );
	
#endif
	
	if ( err )
	{
		return;
	}
	
	#undef SETFULLSCREEN
	
	CGLSetCurrentContext( context );
	
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	/*
		Leave GL_TEXTURE_MAG_FILTER set to its default of GL_LINEAR for now.
		Perhaps in the future we'll have a switch to use GL_NEAREST instead.
		
		We don't have any cases of minification currently, so it doesn't
		matter what GL_TEXTURE_MIN_FILTER is set to.
	*/
	
	glViewport( (int) bounds.origin.x,
	            (int) bounds.origin.y,
	            (int) bounds.size.width,
	            (int) bounds.size.height );
}

CGL_blitter::~CGL_blitter()
{
	// Clear and flush twice to ensure we get a black screen before exit.
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	free( texture_data );
	
	CGLClearDrawable( context );
	CGLDestroyContext( context );
}

void CGL_blitter::prep( int stride, int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0, width, 0, height, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	image_width  = width;
	image_height = height;
	
	tex_width  = image_width;
	tex_height = image_height;
	
	tex_x1 = image_width;
	tex_y1 = image_height;
	
	glTexImage2D( texture_target,
	              0,
	              GL_LUMINANCE,
	              tex_width,
	              tex_height,
	              0,
	              GL_LUMINANCE,
	              GL_UNSIGNED_BYTE,
	              0 );  // NULL
	
	texture_data = (uint8_t*) malloc( tex_width * tex_height );
}

void CGL_blitter::blit( const void* src_addr )
{
	const int n_octets = image_width * image_height / 8u;
	
	transcode_8x_1bpp_to_8bpp( src_addr, texture_data, n_octets );
	
	glBindTexture( texture_target, texture );
	
	glTexSubImage2D( texture_target,
	                 0,
	                 0,
	                 0,
	                 image_width,
	                 image_height,
	                 GL_LUMINANCE,
	                 GL_UNSIGNED_BYTE,
	                 texture_data );
	
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBindTexture( texture_target, texture );
	glEnable( texture_target );
	glBegin( GL_QUADS );
	
	const int width  = image_width;
	const int height = image_height;
	
	glTexCoord2i( 0,     0      ); glVertex2i( 0,     height );  // top left
	glTexCoord2i( width, 0      ); glVertex2i( width, height );  // top right
	glTexCoord2i( width, height ); glVertex2i( width, 0      );  // bottom right
	glTexCoord2i( 0,     height ); glVertex2i( 0,     0      );  // bottom left
	
	glEnd();
	glDisable( texture_target );
	glBindTexture( texture_target, 0 );
	
	CGLFlushDrawable( context );  // implicitly calls glFlush()
}
