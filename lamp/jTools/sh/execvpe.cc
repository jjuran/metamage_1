/*
	execvpe.cc
	----------
*/

#include "execvpe.hh"

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#ifdef __APPLE__

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
	const size_t filename_length = strlen( filename );
	
	const char* pathVar = getpath();
	
	while ( true )
	{
		const char* separator = find_in_str( pathVar, ':' );
		
		size_t dir_path_length = separator - pathVar;
		
		// Empty PATH element means current directory
		const bool has_separator = dir_path_length != 0;
		
		if ( has_separator  &&  pathVar[ dir_path_length - 1 ] == '/' )
		{
			--dir_path_length;
		}
		
		const size_t filename_offset = dir_path_length + has_separator;
		
		const size_t path_length = filename_offset + filename_length;
		
		if ( path_length + 1 > buffer_length )
		{
			goto next;
		}
		
		if ( has_separator )
		{
			memcpy( path, pathVar, dir_path_length );
			
			path[ dir_path_length ] = '/';
		}
		
		memcpy( path + filename_offset, filename, filename_length + 1 );
		
		struct stat sb;
		
		if ( stat( path, &sb ) == 0  &&  sb.st_mode & S_IXUSR )
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

int execvpe( const char* file, char* const argv[], char* const envp[] )
{
	char path[ 4096 ];
	
	if ( strchr( file, '/' ) == NULL )
	{
		if ( -1 == lookup_path( file, path, sizeof path ) )
		{
			errno = ENOENT;
			
			return -1;
		}
		
		file = path;
	}
	
	return execve( file, argv, envp );
}

#endif
