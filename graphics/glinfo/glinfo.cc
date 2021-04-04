/*
	glinfo.cc
	---------
*/

// OpenGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif
#ifndef __gl_h_
#include <GL/gl.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdio.h>

// glinfo
#include "bitmapped_texture_check.hh"
#include "context.hh"


static
void print( const char* prefix, GLenum selector )
{
	if ( const GLubyte* s = glGetString( selector ) )
	{
		printf( "%s %s\n", prefix, s ); 
	}
}

int main()
{
	if ( int err = create_context() )
	{
		fprintf( stderr, "Error creating context: %d\n", err );
		return 1;
	}
	
	print( "Vendor:  ", GL_VENDOR                   );
	print( "Renderer:", GL_RENDERER                 );
	print( "Version: ", GL_VERSION                  );
	
#ifdef GL_SHADING_LANGUAGE_VERSION
	
	print( "GLSL:    ", GL_SHADING_LANGUAGE_VERSION );
	
#endif
	
	if ( const GLubyte* s = glGetString( GL_EXTENSIONS ) )
	{
		printf( "\n" "Extensions:\n" );
		fflush( stdout );
		
		const GLubyte* p = s;
		
		while ( *p )
		{
			const GLubyte* q = p;
			
			while ( *p != '\0'  &&  *p++ != ' ' )  continue;
			
			write( STDOUT_FILENO, "    ", 4 );
			write( STDOUT_FILENO, q, p - q );
			write( STDOUT_FILENO, "\n", 1 );
		}
	}
	
	check_bitmapped_textures();
	
	destroy_context();
}
