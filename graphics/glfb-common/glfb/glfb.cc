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

typedef unsigned char Byte;

bool overlay_enabled;

bool cursor_enabled;
bool cursor_visible;

int cursor_x;
int cursor_y;

int hotspot_dx;
int hotspot_dy;

static Byte palette[ 4 * 256 ];

static int palette_size;

static int image_width;
static int image_height;
static int image_depth;

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

static GLenum cursor_data_format = GL_LUMINANCE;
static GLenum cursor_data_type   = GL_UNSIGNED_BYTE;

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static Byte* screen_texture_data;
static Byte* cursor_texture_data;

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
		              cursor_data_format,
		              cursor_data_type,
		              0 );  // NULL
		
		init_texture( cursor_mask_texture );
		
		glTexImage2D( texture_target,
		              0,
		              GL_LUMINANCE,
		              cursor_width,
		              cursor_height,
		              0,
		              cursor_data_format,
		              cursor_data_type,
		              0 );  // NULL
		
		size_t n_bytes = cursor_width * cursor_height * 2;
		
		cursor_texture_data = (Byte*) malloc( n_bytes );
	}
	
	init_texture( screen_texture );
}

void set_dimensions( int width, int height, int depth )
{
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0, width, 0, height, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	image_width  = width;
	image_height = height;
	image_depth  = depth;
	
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	tex_width  = image_width;
	tex_height = image_height;
	
#else
	
	int greater = width > height ? width : height;
	
	tex_width  =
	tex_height = binary_power_up( greater );
	
#endif
	
	size_t buffer_size = tex_width * tex_height;
	
	if ( depth > 8  ||  palette_size > 0 )
	{
		/*
			Switch to color output if the bit depth is 16 or 32,
			or if we have a palette (even for a bit depth of 1).
		*/
		
		texture_format = GL_RGBA;
		
		buffer_size *= 4;
	}
	
	glTexImage2D( texture_target,
	              0,
	              GL_RGB,
	              tex_width,
	              tex_height,
	              0,
	              texture_format,
	              texture_type,
	              0 );  // NULL
	
	free( screen_texture_data );
	
	screen_texture_data = (Byte*) malloc( buffer_size );
}

void set_palette( const unsigned short* colors, int n )
{
	palette_size = n;
	
	Byte* p = palette;
	
	const unsigned short* color = colors;
	
	for ( int i = 0;  i < n;  ++i )
	{
		++color;
		
		*p++ = *color++ >> 8;
		*p++ = *color++ >> 8;
		*p++ = *color++ >> 8;
		*p++ = 0xFF;
	}
}

static inline
Byte* write_pixel_from_palette( Byte* dst, Byte index )
{
	const Byte* entry = &palette[ index * 4 ];
	
	*dst++ = *entry++;
	*dst++ = *entry++;
	*dst++ = *entry++;
	*dst++ = *entry++;
	
	return dst;
}

static inline
void transcode_8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n )
{
	/*
		This is actually inverted:  The 0 and 1 bits code for $FF and $00,
		for transcoding classic Mac OS (black-on-white) binary image data to
		conventional white-on-black grayscale as used in OpenGL.
	*/
	
	transcode::_8x_1bpp_to_8bpp( src, dst, n, 0xFF, 0x00 );
}

static inline
void transcode_1bpp_paletted( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		Byte octet = *src++;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			octet = octet << 1 | octet >> 7;  // rotate left 1 bit
			
			dst = write_pixel_from_palette( dst, octet & 0x1 );
		}
	}
}

static inline
void transcode_4x_2bpp_to_8bpp( const Byte* src, Byte* dst, int n )
{
	const Byte ramp[] = { 0xFF, 0xAA, 0x55, 0x00 };
	
	while ( n-- > 0 )
	{
		Byte octet = *src++;
		
		for ( int i = 0;  i < 4;  ++i )
		{
			octet = octet << 2 | octet >> 6;  // rotate left 2 bits
			
			*dst++ = ramp[ octet & 0x3 ];
		}
	}
}

static inline
void transcode_2bpp_paletted( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		Byte octet = *src++;
		
		for ( int i = 0;  i < 4;  ++i )
		{
			octet = octet << 2 | octet >> 6;  // rotate left 2 bits
			
			dst = write_pixel_from_palette( dst, octet & 0x3 );
		}
	}
}

static inline
void transcode_2x_4bpp_to_8bpp( const Byte* src, Byte* dst, int n )
{
	const Byte ramp[] =
	{
		0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
		0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
	};
	
	while ( n-- > 0 )
	{
		Byte octet = *src++;
		
		*dst++ = ramp[ octet >>  4 ];
		*dst++ = ramp[ octet & 0xF ];
	}
}

static
void transcode_4bpp_paletted( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		Byte octet = *src++;
		
		Byte i = octet >>  4;
		Byte j = octet & 0xF;
		
		dst = write_pixel_from_palette( dst, i );
		dst = write_pixel_from_palette( dst, j );
	}
}

static inline
void transcode_inverted( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		*dst++ = ~*src++;
	}
}

static
void transcode_8bpp_paletted( const Byte* src, Byte* dst, int n )
{
	while ( n-- > 0 )
	{
		dst = write_pixel_from_palette( dst, *src++ );
	}
}

static
void transcode_argb_1555( const Byte* src, Byte* dst, int n )
{
	while ( (n -= 2) > 0 )
	{
		Byte high = *src++;
		Byte low  = *src++;
		
		// ARRRRRGG GGGBBBBB
		
		Byte a = (int8_t) high >> 7;
		Byte r = (high << 1) & 0xf8;
		Byte g = (high << 6) | ((low >> 2) & 0xf8);
		Byte b = (low  << 3) & 0xf8;
		
		r |= r >> 5;
		g |= g >> 5;
		b |= b >> 5;
		
		*dst++ = r;
		*dst++ = g;
		*dst++ = b;
		*dst++ = a;
	}
}

static
void transcode_argb_8888( const Byte* src, Byte* dst, int n )
{
	while ( (n -= 4) > 0 )
	{
		Byte a = *src++;
		Byte r = *src++;
		Byte g = *src++;
		Byte b = *src++;
		
		*dst++ = r;
		*dst++ = g;
		*dst++ = b;
		*dst++ = a;
	}
}

void set_screen_image( const void* src_addr )
{
	const Byte* src = (const Byte*) src_addr;
	
	const int n_octets = image_width * image_height * image_depth / 8;
	
	switch ( image_depth )
	{
		case 1:
			if ( palette_size > 0 )
			{
				transcode_1bpp_paletted( src, screen_texture_data, n_octets );
			}
			else
			{
				transcode_8x_1bpp_to_8bpp( src, screen_texture_data, n_octets );
			}
			break;
		
		case 2:
			if ( palette_size > 0 )
			{
				transcode_2bpp_paletted( src, screen_texture_data, n_octets );
			}
			else
			{
				transcode_4x_2bpp_to_8bpp( src, screen_texture_data, n_octets );
			}
			break;
		
		case 4:
			if ( palette_size > 0 )
			{
				transcode_4bpp_paletted( src, screen_texture_data, n_octets );
			}
			else
			{
				transcode_2x_4bpp_to_8bpp( src, screen_texture_data, n_octets );
			}
			break;
		
		case 8:
			if ( palette_size > 0 )
			{
				transcode_8bpp_paletted( src, screen_texture_data, n_octets );
			}
			else
			{
				transcode_inverted( src, screen_texture_data, n_octets );
			}
			break;
		
		case 16:
			transcode_argb_1555( src, screen_texture_data, n_octets );
			break;
		
		case 32:
			transcode_argb_8888( src, screen_texture_data, n_octets );
			break;
	}
	
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
		
		const Byte* bits = (const Byte*) src_addr;
		
		transcode_8x_1bpp_to_8bpp( bits, cursor_texture_data, n_octets );
		
		glBindTexture( texture_target, cursor_mask_texture );
		
		glTexSubImage2D( texture_target,
		                 0,
		                 0,
		                 0,
		                 cursor_width,
		                 cursor_height,
		                 cursor_data_format,
		                 cursor_data_type,
		                 cursor_texture_data + 256 );
		
		glBindTexture( texture_target, cursor_face_texture );
		
		glTexSubImage2D( texture_target,
		                 0,
		                 0,
		                 0,
		                 cursor_width,
		                 cursor_height,
		                 cursor_data_format,
		                 cursor_data_type,
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
