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


static int image_width;
static int image_height;

static int tex_width;
static int tex_height;

#ifdef GL_TEXTURE_RECTANGLE_ARB

const GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB;

#define glTexCoord_ glTexCoord2i

static int tex_x1;
static int tex_y1;

#else

const GLenum texture_target = GL_TEXTURE_2D;

#define glTexCoord_ glTexCoord2f

static float tex_x1;
static float tex_y1;

#endif

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static uint8_t* texture_data;

static CGLContextObj context;
static GLuint        texture;

static inline
unsigned binary_power_up( unsigned x )
{
	--x;
	
	int power = 1;
	
	while ( x >>= 1 )
	{
		++power;
	}
	
	return 1 << power;
}

CGL_blitter::CGL_blitter( CGDirectDisplayID id, CGRect bounds )
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
	
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	/*
		Leave GL_TEXTURE_MAG_FILTER set to its default of GL_LINEAR for now.
		Perhaps in the future we'll have a switch to use GL_NEAREST instead.
		
		We don't have any cases of minification currently, so it doesn't
		matter what GL_TEXTURE_MIN_FILTER is set to.
	*/
	
#ifndef GL_TEXTURE_RECTANGLE_ARB
	
	// Leave GL_TEXTURE_BASE_LEVEL set to its default of zero.
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	
#endif
	
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
	
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	tex_width  = image_width;
	tex_height = image_height;
	
	tex_x1 = image_width;
	tex_y1 = image_height;
	
#else
	
	int greater = width > height ? width : height;
	
	tex_width  =
	tex_height = binary_power_up( greater );
	
	tex_x1 = 1.0 * image_width  / tex_width;
	tex_y1 = 1.0 * image_height / tex_height;
	
#endif
	
	glTexImage2D( texture_target,
	              0,
	              GL_LUMINANCE,
	              tex_width,
	              tex_height,
	              0,
	              texture_format,
	              texture_type,
	              0 );  // NULL
	
	texture_data = (uint8_t*) malloc( tex_width * tex_height );
}

void CGL_blitter::blit( const void* src_addr )
{
	const int n_octets = image_width * image_height / 8u;
	
	transcode_8x_1bpp_to_8bpp( src_addr, texture_data, n_octets );
	
	src_addr = texture_data;
	
	glBindTexture( texture_target, texture );
	
	glTexSubImage2D( texture_target,
	                 0,
	                 0,
	                 0,
	                 image_width,
	                 image_height,
	                 texture_format,
	                 texture_type,
	                 src_addr );
	
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBindTexture( texture_target, texture );
	glEnable( texture_target );
	glBegin( GL_QUADS );
	
	const int width  = image_width;
	const int height = image_height;
	
	glTexCoord_( 0,      0      ); glVertex2i( 0,     height );  // top left
	glTexCoord_( tex_x1, 0      ); glVertex2i( width, height );  // top right
	glTexCoord_( tex_x1, tex_y1 ); glVertex2i( width, 0      );  // bottom right
	glTexCoord_( 0,      tex_y1 ); glVertex2i( 0,     0      );  // bottom left
	
	glEnd();
	glDisable( texture_target );
	glBindTexture( texture_target, 0 );
	
	CGLFlushDrawable( context );  // implicitly calls glFlush()
}
