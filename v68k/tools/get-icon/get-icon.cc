/*
	get-icon.cc
	-----------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>


const int sizeof_icon = 256;

static
Handle get_icon()
{
	Handle icon = Get1Resource( 'ICN#', 128 );
	
	if ( ! icon )
	{
		icon = Get1Resource( 'ICN#', 256 );  // Lode Runner...
	}
	
	if ( ! icon )
	{
		icon = Get1Resource( 'ICN#', 1000 );  // Lemmings...
	}
	
	return icon;
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
		if ( Handle icon = get_icon() )
		{
			Size size = GetHandleSize( icon );
			
			if ( size == sizeof_icon )
			{
				ssize_t n = write( STDOUT_FILENO, *icon, sizeof_icon );
				
				if ( n == sizeof_icon )
				{
					return 0;
				}
			}
		}
	}
	
	return 1;
}
