/*
	app.cc
	------
*/

// Mac OS
#ifndef __PACKAGES__
#include <Packages.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// chars
#include "conv/mac_utf8.hh"


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
	
	using conv::mac_from_utf8;
	
	const char* path = argv[ 1 ];
	
	const size_t len = strlen( path );
	
	InitResources();
	InitAllPacks();
	
	Str31 appName;
	
	size_t n_utf8_bytes = mac_from_utf8( (char*) appName + 1, 31, path, len );
	
	if ( n_utf8_bytes > 31 )
	{
		return 2;
	}
	
	appName[ 0 ] = n_utf8_bytes;
	
	LaunchParamBlockRec pb = { appName };
	
	Launch( &pb );
	
	return 1;
}
