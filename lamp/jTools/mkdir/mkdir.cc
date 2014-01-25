/*	========
 *	mkdir.cc
 *	========
 */

// Standard C/C++
#include <cstdio>

// Standard C
#include <errno.h>

// POSIX
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


static int failures = 0;


static void make_dir( const char* path )
{
	int f = mkdir( path, 0700 );
	
	if ( f < 0 )
	{
		more::perror( "mkdir", path );
		
		++failures;
	}
}

int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		more::perror( "mkdir", "missing arguments", 0 );
		
		return 1;
	}
	
	// Try to make each directory.  Return whether any errors occurred.
	
	while ( *++argv != NULL )
	{
		make_dir( *argv );
	}
	
	return (failures == 0) ? 0 : 1;
}
