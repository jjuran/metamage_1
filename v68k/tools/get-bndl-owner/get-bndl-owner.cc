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
	
	const char* path = argv[ 1 ];
	
	const size_t len = strlen( path );
	
	if ( len > 31 )
	{
		return 2;
	}
	
	Str31 appName;
	
	unsigned char* p = appName;
	
	*p++ = len;
	
	memcpy( p, path, len );
	
	short refnum = OpenResFile( appName );
	
	if ( refnum > 0 )
	{
		if ( Handle bndl = get_bndl() )
		{
			Size size = GetHandleSize( bndl );
			
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
