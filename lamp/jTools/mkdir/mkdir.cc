/*
	mkdir.cc
	--------
*/

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


static int failures = 0;


static
void make_dir( const char* path, int ignored_err = 0 )
{
	int f = mkdir( path, 0700 );
	
	if ( f < 0  &&  errno != ignored_err )
	{
		more::perror( "mkdir", path );
		
		++failures;
	}
}

static
void make_dirs( char* path, char* end )
{
	char* p = end;
	
	while ( p > path  &&  *--p != '/' ) continue;
	
	if ( p != path )
	{
		*p = '\0';
		
		make_dirs( path, p );
		
		*p = '/';
	}
	
	make_dir( path, EEXIST );
}

static
void make_dirs( char* path )
{
	const size_t length = strlen( path );
	
	char* end = path + length;
	
	make_dirs( path, end );
}

int main( int argc, char *const *argv )
{
	if ( *argv == NULL )
	{
		return 0;
	}
	
	// Try to make each directory.  Return whether any errors occurred.
	
	bool prefixes = false;
	
	while ( *++argv != NULL )
	{
		if ( strcmp( *argv, "-p" ) == 0 )
		{
			prefixes = true;
			
			continue;
		}
		
		--argv;
		
		break;
	}
	
	if ( *argv == NULL )
	{
		more::perror( "mkdir", "missing arguments", 0 );
		
		return 1;
	}
	
	// Try to make each directory.  Return whether any errors occurred.
	
	while ( *++argv != NULL )
	{
		if ( prefixes )
		{
			make_dirs( *argv );
		}
		else
		{
			make_dir( *argv );
		}
	}
	
	return (failures == 0) ? 0 : 1;
}
