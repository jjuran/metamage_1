/*
	which.cc
	--------
*/

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// must
#include "must/write.h"


#ifdef __RELIX__
// Relix
#include "relix/alloca.h"
#define ALLOC( x )  signalling_alloca( x )
#else
#define ALLOC( x )  malloc( x )
#endif


#pragma exceptions off


static
size_t get_max_dir_length( const char* path )
{
	size_t result = 0;
	
	const char* p = path;
	
	while ( *p != '\0' )
	{
		const char* q = p;
		
		while ( *q != ':'  &&  *q != '\0' )
		{
			++q;
		}
		
		const size_t len = q - p;
		
		if ( len > result )
		{
			result = len;
		}
		
		p = q + !!*q;
	}
	
	return result;
}

static
size_t get_max_program_length( char** argv )
{
	size_t result = 0;
	
	while ( *++argv != NULL )
	{
		const size_t len = strlen( *argv );
		
		if ( len > result )
		{
			result = len;
		}
	}
	
	return result;
}

int main( int argc, char** argv )
{
	// Check for sufficient number of args
	if ( argc <= 1 )
	{
		return 1;
	}
	
	const char* path = getenv( "PATH" );
	
	if ( path == NULL )
	{
		path = "/usr/bin:/bin";
	}
	
	const size_t max_dir_length = get_max_dir_length( path );
	
	const size_t max_program_length = get_max_program_length( argv );
	
	const size_t buffer_size = max_dir_length + 1 + max_program_length + 1;
	
	char* const buffer = (char*) ALLOC( buffer_size );
	
	char* end = buffer + buffer_size;
	
	*--end = '\0';
	
	bool failed = false;
	
	
	while ( *++argv != NULL )
	{
		const char* program = *argv;
		
		const size_t program_len = strlen( program );
		
		char* mark = end - program_len;
		
		mempcpy( mark, program, program_len );
		
		*--mark = '/';
		
		bool found = false;
		
		const char* p = path;
		
		while ( *p != '\0' )
		{
			const char* q = p;
			
			while ( *q != ':'  &&  *q != '\0' )
			{
				++q;
			}
			
			const size_t dir_len = q - p;
			
			char* begin = mark - dir_len;
			
			mempcpy( begin, p, dir_len );
			
			struct stat sb;
			
			int status = stat( begin, &sb );
			
			int mask = S_IFMT | S_IXUSR;
			
			if ( status == 0  &&  (sb.st_mode & mask) == (S_IFREG | S_IXUSR) )
			{
				found = true;
				
				*end++ = '\n';
				
				must_write( STDOUT_FILENO, begin, end - begin );
				
				break;
			}
			
			p = q + !!*q;
		}
		
		if ( !found )
		{
			failed = true;
		}
	}
	
	return failed ? 1 : 0;
}
