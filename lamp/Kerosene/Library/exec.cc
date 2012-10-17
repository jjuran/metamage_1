/*	=======
 *	exec.cc
 *	=======
 */

// Standard C
#include "errno.h"
#include <stdarg.h>
#include "stdlib.h"

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Iota
#include "iota/strings.hh"


#pragma exceptions off


extern "C" char** environ;


static inline void inscribe_decimal( int x, char* s )
{
	char units[] = "9876543210";
	
	char* p = units + STRLEN( "9876543210" );
	
	if ( x == 0 )
	{
		--p;
	}
	else
	{
		while ( x != 0 )
		{
			*--p = '0' + x % 10;
			
			x /= 10;
		}
	}
	
	std::strcpy( s, p );
}

int fexecve( int fd, char *const argv[], char *const envp[] )
{
	char path[] = "/dev/fd/9876543210";
	
	inscribe_decimal( fd, path + STRLEN( "/dev/fd/" ) );
	
	return execve( path, argv, envp );
}

int execv( const char* path, char* const* argv )
{
	return execve( path, argv, environ );
}

static inline const char* getpath()
{
	if ( const char* path = getenv( "PATH" ) )
	{
		return path;
	}
	
	return ".";
}

static inline const char* find_in_str( const char* s, char c )
{
	while ( *s != '\0'  &&  *s != c )
	{
		++s;
	}
	
	return s;
}

static int lookup_path( const char* filename, char* path, size_t buffer_length )
{
	const std::size_t filename_length = std::strlen( filename );
	
	const char* pathVar = getpath();
	
	while ( true )
	{
		const char* separator = find_in_str( pathVar, ':' );
		
		std::size_t dir_path_length = separator - pathVar;
		
		// Empty PATH element means current directory
		const bool has_separator = dir_path_length != 0;
		
		if ( has_separator  &&  pathVar[ dir_path_length - 1 ] == '/' )
		{
			--dir_path_length;
		}
		
		const std::size_t filename_offset = dir_path_length + has_separator;
		
		const std::size_t path_length = filename_offset + filename_length;
		
		if ( path_length + 1 > buffer_length )
		{
			goto next;
		}
		
		if ( has_separator )
		{
			std::memcpy( path, pathVar, dir_path_length );
			
			path[ dir_path_length ] = '/';
		}
		
		std::memcpy( path + filename_offset, filename, filename_length + 1 );
		
		struct stat sb;
		
		int status = stat( path, &sb );
		
		if ( status == 0  &&  sb.st_mode & S_IXUSR )
		{
			return 0;
		}
		
	next:
		
		if ( *separator == '\0' )
		{
			break;
		}
		
		pathVar = separator + 1;
	}
	
	return -1;
}

int execvp( const char* file, char* const argv[] )
{
	char path[ 4096 ];
	
	if ( std::strchr( file, '/' ) == NULL )
	{
		if ( -1 == lookup_path( file, path, sizeof path ) )
		{
			errno = ENOENT;
			
			return -1;
		}
		
		file = path;
	}
	
	return execv( file, argv );
}

int execle( const char* path, const char* arg0, ... )
{
	va_list va;
	
	va_start( va, arg0 );
	
	while ( const char* arg = va_arg( va, const char * ) )
	{
	}
	
	char* const* envp = va_arg( va, char* const* );
	
	va_end( va );
	
	char* const* argv = (char**) &arg0;
	
	return execve( path, argv, envp );
}

int execl( const char* path, const char* arg0, ... )
{
	return execv( path, (char**) &arg0 );
}

int execlp( const char* file, const char* arg0, ... )
{
	return execvp( file, (char**) &arg0 );
}

