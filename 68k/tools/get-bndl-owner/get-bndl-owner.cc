/*
	get-bndl-owner.cc
	-----------------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// chars
#include "conv/mac_utf8.hh"


static inline
Handle get_bndl()
{
	/*
		There should only be one 'BNDL' resource.
		
		Most applications have a 'BNDL' ID 128, but Lode Runner's is ID 256.
		Thanks, Glenn!
	*/
	
	return Get1IndResource( 'BNDL', 1 );
}

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		return 0;
	}
	
	using conv::mac_from_utf8_nothrow;
	
	const char* path = argv[ 1 ];
	
	const size_t len = strlen( path );
	
	Str31 appName;
	
	char* buffer = (char*) appName + 1;
	
	size_t n_bytes = mac_from_utf8_nothrow( buffer, 31, path, len );
	
	if ( n_bytes > 31 )
	{
		return 2;
	}
	
	appName[ 0 ] = n_bytes;
	
	short refnum = OpenResFile( appName );
	
	if ( refnum > 0 )
	{
		if ( Handle bndl = get_bndl() )
		{
			Size size = mac::glue::GetHandleSize_raw( bndl );
			
			if ( size >= sizeof (OSType) )
			{
				ssize_t n = write( STDOUT_FILENO, *bndl, sizeof (OSType) );
				
				if ( n == sizeof (OSType) )
				{
					return 0;
				}
			}
		}
	}
	
	return 1;
}
