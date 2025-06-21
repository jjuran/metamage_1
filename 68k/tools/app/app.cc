/*
	app.cc
	------
*/

// Mac OS
#ifndef __PACKAGES__
#include <Packages.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// chars
#include "conv/mac_utf8.hh"


#pragma exceptions off


struct LaunchParamBlockRec
{
	void* appName;
	short reserved2;
	// ...
};

static QDGlobals qd;

static inline
short asm Launch( void* pb : __A0 ) : __D0
{
	DC.W     0xA9F2  // _Launch
}

static inline
void paint_desktop()
{
	InitGraf( &qd.thePort );
	
	GrafPort port;
	
	OpenPort( &port );
	
	FillRoundRect( &port.portRect, 16, 16, &qd.gray );
	
	ClosePort( &port );
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
	
	paint_desktop();
	
	InitResources();
	InitAllPacks();
	
	Byte appName[ 33 ];
	
	size_t n = mac_from_utf8_nothrow( (char*) appName + 1, 32, path, len );
	
	if ( n > 31 )
	{
		return 2;
	}
	
	appName[ 0 ] = n;
	
	LaunchParamBlockRec pb = { appName };
	
	Launch( &pb );
	
	return 1;
}
