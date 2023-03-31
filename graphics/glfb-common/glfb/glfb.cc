/*
	glfb.cc
	-------
*/

#include "glfb/glfb.hh"

// OpenGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif
#ifndef __gl_h_
#include <GL/gl.h>
#endif

// Standard C
#include <stdlib.h>

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"


namespace glfb
{

bool overlay_enabled;

static int image_width;
static int image_height;

static int tex_width;
static int tex_height;

#ifdef GL_TEXTURE_RECTANGLE_ARB

const GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB;

#else

const GLenum texture_target = GL_TEXTURE_RECTANGLE_EXT;

#endif

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static uint8_t* texture_data;

static GLuint texture;


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

void initialize()
{
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	/*
		Leave GL_TEXTURE_MAG_FILTER set to its default of GL_LINEAR for now.
		Perhaps in the future we'll have a switch to use GL_NEAREST instead.
		
		We don't have any cases of minification currently, so it doesn't
		matter what GL_TEXTURE_MIN_FILTER is set to.
	*/
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void set_dimensions( int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0, width, 0, height, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	image_width  = width;
	image_height = height;
	
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	tex_width  = image_width;
	tex_height = image_height;
	
#else
	
	int greater = width > height ? width : height;
	
	tex_width  =
	tex_height = binary_power_up( greater );
	
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
	
	free( texture_data );
	
	texture_data = (uint8_t*) malloc( tex_width * tex_height );
}

void set_screen_image( const void* src_addr )
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
}

void blit( const void* src_addr )
{
	set_screen_image( src_addr );
	
	render();
}

void render()
{
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
	
	if ( overlay_enabled )
	{
		glEnable( GL_BLEND );
		
		glColor4f( 0.50, 0.50, 0.50, 0.50 );
		glBegin( GL_QUADS );
		
		glVertex2i( 0,     height );  // top left
		glVertex2i( width, height );  // top right
		glVertex2i( width, 0      );  // bottom right
		glVertex2i( 0,     0      );  // bottom left
		
		glEnd();
		glColor4f( 1, 1, 1, 1 );
		
		glDisable( GL_BLEND );
	}
	
	glBindTexture( texture_target, 0 );
}

void terminate()
{
	free( texture_data );
}

}
