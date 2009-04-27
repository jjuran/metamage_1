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

// Standard C++
#include <string>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Iota
#include "iota/strings.hh"


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

int fexecve( int fd, const char *const argv[], const char *const envp[] )
{
	char path[] = "/dev/fd/9876543210";
	
	inscribe_decimal( fd, path + STRLEN( "/dev/fd/" ) );
	
	return execve( path, argv, envp );
}

static std::string LookupPath( const char* filename )
{
	const char* pathVar = getenv( "PATH" );
	
	if ( pathVar == NULL )
	{
		return "";
	}
	
	const char* pathEnd = pathVar + std::strlen( pathVar );
	
	while ( pathVar < pathEnd )
	{
		const char* separator = std::strchr( pathVar, ':' );
		
		if ( separator == NULL )
		{
			separator = pathEnd;
		}
		
		// Watch out for empty path elements (e.g. "/bin:/sbin:" -- last is empty)
		if ( separator != pathVar )
		{
			std::string dir( pathVar, separator - pathVar );
			
			std::string tryPath = dir + "/" + filename;
			
			struct ::stat sb;
			int status = stat( tryPath.c_str(), &sb );
			
			if ( status == 0  &&  sb.st_mode & S_IXUSR )
			{
				return tryPath;
			}
		}
		
		// If we're at the end, then this sets pathVar > pathEnd
		pathVar = separator + 1;
	}
	
	return "";
}

int execl( const char* path, const char* arg0, ... )
{
	return execv( path, &arg0 );
}

int execle( const char* path, const char* arg0, ... )
{
	va_list va;
	
	va_start( va, arg0 );
	
	while ( const char* arg = va_arg( va, const char * ) )
	{
	}
	
	const char* const* envp = va_arg( va, const char* const* );
	
	va_end( va );
	
	const char* const* argv = &arg0;
	
	return execve( path, argv, envp );
}

int execlp( const char* file, const char* arg0, ... )
{
	return execvp( file, &arg0 );
}

int execvp( const char* file, const char* const argv[] )
{
	std::string path;
	
	if ( std::strchr( file, '/' ) == NULL )
	{
		path = LookupPath( file );
		
		if ( path == "" )
		{
			errno = ENOENT;
			
			return -1;
		}
		
		file = path.c_str();
	}
	
	return execv( file, argv );
}

