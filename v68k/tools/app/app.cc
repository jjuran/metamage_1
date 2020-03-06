/*
	app.cc
	------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>


struct LaunchParamBlockRec
{
	void* appName;
	short reserved2;
	// ...
};

static inline
short asm Launch( void* pb : __A0 ) : __D0
{
	DC.W     0xA9F2  // _Launch
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
	
	InitResources();
	
	Str31 appName;
	
	unsigned char* p = appName;
	
	*p++ = len;
	
	memcpy( p, path, len );
	
	LaunchParamBlockRec pb = { appName };
	
	Launch( &pb );
	
	return 1;
}
