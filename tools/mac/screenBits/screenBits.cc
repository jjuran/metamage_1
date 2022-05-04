/*
	screenBits.cc
	-------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Standard C
#include <stdio.h>

// mac-qd-utils
#include "mac_qd/globals/screenBits.hh"


int main( int argc, char** argv )
{
#if TARGET_API_MAC_CARBON
	
	const BitMap& screenBits = mac::qd::screenBits();
	
#else
	
	GrafPort port;
	
	OpenPort( &port );
	
	const BitMap screenBits = port.portBits;
	
	ClosePort( &port );
	
#endif
	
	const Rect& bounds = screenBits.bounds;
	
	printf( "%p (%d)  [%d,%d - %d,%d]\n",
	        screenBits.baseAddr,
	        screenBits.rowBytes,
	        bounds.left, bounds.top, bounds.right, bounds.bottom );
	
	return 0;
}
