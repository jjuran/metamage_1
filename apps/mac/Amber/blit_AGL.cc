/*
	blit_AGL.cc
	-----------
*/

#include "blit_AGL.hh"

// Mac OS X
#ifdef __APPLE__
#include <AGL/AGL.h>
#include <OpenGL/OpenGL.h>
#endif

// OpenGL
#include <OpenGL/gl.h>

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"


static int image_width;
static int image_height;

static int tex_width;
static int tex_height;

const GLenum texture_target = GL_TEXTURE_2D;

#define glTexCoord_ glTexCoord2f

static float tex_x1;
static float tex_y1;

static bool bitmaps_make_textures;

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static uint8_t* texture_data;

static AGLContext context;
static GLuint     texture;

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
	
	/*
		Don't even check for bitmap support if we don't have rectangle
		textures.  It's always supported with 2D textures, but not in the
		older OpenGL hardware -- and falling back to software rendering
		kills performance.
	*/
	
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	/*
		Leave GL_TEXTURE_MAG_FILTER set to its default of GL_LINEAR for now.
		Perhaps in the future we'll have a switch to use GL_NEAREST instead.
		
		We don't have any cases of minification currently, so it doesn't
		matter what GL_TEXTURE_MIN_FILTER is set to.
	*/
	
	// Leave GL_TEXTURE_BASE_LEVEL set to its default of zero.
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
}

void destroy_AGL_context()
{
	free( texture_data );
	
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

void set_AGL_geometry( int stride, int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0, width, 0, height, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	image_width  = width;
	image_height = height;
	
	int greater = width > height ? width : height;
	
	tex_width  =
	tex_height = binary_power_up( greater );
	
	tex_x1 = 1.0 * image_width  / tex_width;
	tex_y1 = 1.0 * image_height / tex_height;
	
	glTexImage2D( texture_target,
	              0,
	              GL_LUMINANCE,
	              tex_width,
	              tex_height,
	              0,
	              texture_format,
	              texture_type,
	              0 );  // NULL
	
	if ( ! bitmaps_make_textures )
	{
		texture_data = (uint8_t*) malloc( tex_width * tex_height );
	}
}

void blit_AGL( const void* src_addr )
{
	if ( ! bitmaps_make_textures )
	{
		const int n_octets = image_width * image_height / 8u;
		
		transcode_8x_1bpp_to_8bpp( src_addr, texture_data, n_octets );
		
		src_addr = texture_data;
	}
	
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
	
	glFlush();
	aglSwapBuffers( context );
}
