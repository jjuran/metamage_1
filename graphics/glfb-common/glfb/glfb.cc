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

bool cursor_enabled;
bool cursor_visible;

int cursor_x;
int cursor_y;

int hotspot_dx;
int hotspot_dy;

static int image_width;
static int image_height;

static int tex_width;
static int tex_height;

#ifdef GL_TEXTURE_RECTANGLE_ARB

const GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB;

#else

const GLenum texture_target = GL_TEXTURE_RECTANGLE_EXT;

#endif

const int cursor_width  = 16;
const int cursor_height = 16;

static int crsr_x0;
static int crsr_y0;
static int crsr_x1;
static int crsr_y1;

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static uint8_t* screen_texture_data;
static uint8_t* cursor_texture_data;

static GLuint screen_texture;
static GLuint cursor_face_texture;
static GLuint cursor_mask_texture;


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

static
void init_texture( GLuint& texture )
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

void initialize()
{
	if ( cursor_enabled )
	{
		init_texture( cursor_face_texture );
		
		glTexImage2D( texture_target,
		              0,
		              GL_LUMINANCE,
		              cursor_width,
		              cursor_height,
		              0,
		              texture_format,
		              texture_type,
		              0 );  // NULL
		
		init_texture( cursor_mask_texture );
		
		glTexImage2D( texture_target,
		              0,
		              GL_LUMINANCE,
		              cursor_width,
		              cursor_height,
		              0,
		              texture_format,
		              texture_type,
		              0 );  // NULL
		
		size_t n_bytes = cursor_width * cursor_height * 2;
		
		cursor_texture_data = (uint8_t*) malloc( n_bytes );
	}
	
	init_texture( screen_texture );
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
	
	free( screen_texture_data );
	
	screen_texture_data = (uint8_t*) malloc( tex_width * tex_height );
}

void set_screen_image( const void* src_addr )
{
	const int n_octets = image_width * image_height / 8u;
	
	transcode_8x_1bpp_to_8bpp( src_addr, screen_texture_data, n_octets );
	
	src_addr = screen_texture_data;
	
	glBindTexture( texture_target, screen_texture );
	
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

void set_cursor_image( const void* src_addr )
{
	if ( cursor_enabled )
	{
		const int n_octets = cursor_width * cursor_height / 8u * 2;  // 64 bytes
		
		const uint8_t* bits = (const uint8_t*) src_addr;
		
		transcode_8x_1bpp_to_8bpp( bits, cursor_texture_data, n_octets );
		
		glBindTexture( texture_target, cursor_mask_texture );
		
		glTexSubImage2D( texture_target,
		                 0,
		                 0,
		                 0,
		                 cursor_width,
		                 cursor_height,
		                 texture_format,
		                 texture_type,
		                 cursor_texture_data + 256 );
		
		glBindTexture( texture_target, cursor_face_texture );
		
		glTexSubImage2D( texture_target,
		                 0,
		                 0,
		                 0,
		                 cursor_width,
		                 cursor_height,
		                 texture_format,
		                 texture_type,
		                 cursor_texture_data );
		
	}
}

static
void cursor_quad( GLuint texture )
{
	const int width  = cursor_width;
	const int height = cursor_height;
	
	glBindTexture( texture_target, texture );
	
	glEnable( texture_target );
	glBegin( GL_QUADS );
	
	glTexCoord2i( 0,     0      ); glVertex2i( crsr_x0, crsr_y1 );  // top L
	glTexCoord2i( width, 0      ); glVertex2i( crsr_x1, crsr_y1 );  // top R
	glTexCoord2i( width, height ); glVertex2i( crsr_x1, crsr_y0 );  // bottom R
	glTexCoord2i( 0,     height ); glVertex2i( crsr_x0, crsr_y0 );  // bottom L
	
	glEnd();
	glDisable( texture_target );
}

void render()
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBindTexture( texture_target, screen_texture );
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
	
	if ( cursor_enabled  &&  cursor_visible )
	{
		crsr_x0 =          cursor_x - hotspot_dx;
		crsr_y1 = height - cursor_y + hotspot_dy;
		
		crsr_x1 = crsr_x0 + cursor_width;
		crsr_y0 = crsr_y1 - cursor_height;
		
		glEnable( GL_COLOR_LOGIC_OP );
		
		glLogicOp( GL_AND_REVERSE );  cursor_quad( cursor_mask_texture );
		glLogicOp( GL_XOR );          cursor_quad( cursor_face_texture );
		
		glDisable( GL_COLOR_LOGIC_OP );
	}
	
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
	free( screen_texture_data );
}

}
