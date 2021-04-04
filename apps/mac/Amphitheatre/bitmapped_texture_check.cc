/*
	bitmapped_texture_check.cc
	--------------------------
*/

#include "bitmapped_texture_check.hh"

// OpenGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif
#ifndef __gl_h_
#include <GL/gl.h>
#endif

// Standard C
#include <stdint.h>


bool has_bitmapped_textures()
{
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	short   data = 0xAA55;
	GLsizei width = 8;
	GLsizei height = 2;
	GLuint  texture;
	GLenum  texture_target = GL_TEXTURE_RECTANGLE_ARB;
	
	const float palette[] = { 1.0, 0.0 };  // white = 0, black = 1
	
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	
	glPixelMapfv( GL_PIXEL_MAP_I_TO_R, 2, palette );  // first channel is "red"
	
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	glTexImage2D( texture_target,
	              0,
	              GL_LUMINANCE,
	              width,
	              height,
	              0,
	              GL_COLOR_INDEX,
	              GL_BITMAP,
	              &data );  // NULL
	
	uint64_t bits[ 2 ] = {};
	glGetTexImage( texture_target, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bits );
	
	glBindTexture( texture_target, 0 );
	
	glDeleteTextures( 1, &texture );
	
	return bits[ 0 ] != 0;
	
#endif
	
	/*
		GL_TEXTURE_2D always supports populating a texture from a bitmap,
		but if it's not supported by the hardware then we'll get kicked over
		to a software-only renderer, which is way worse than just transcoding
		on the CPU.  Return false to avoid this horrid fate.
	*/
	
	return false;
}
