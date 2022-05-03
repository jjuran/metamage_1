/*
	gdev.cc
	-------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// Standard C
#include <stdio.h>

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

int main( int argc, char** argv )
{
	if ( ! has_color_quickdraw() )
	{
		return 1;
	}
	
	GDHandle main_device = GetMainDevice();
	
	GDHandle gdH = GetDeviceList();
	
	while ( gdH != NULL )
	{
		char mark = gdH == main_device ? '*' : ' ';
		
		const Rect& rect = gdH[0]->gdRect;
		
		printf( "%c %p (%.4x, %d)  [%d,%d - %d,%d]\n", mark, gdH,
		        (uint16_t) gdH[0]->gdFlags, gdH[0]->gdRefNum,
		        rect.left, rect.top, rect.right, rect.bottom );
		
		gdH = GetNextDevice( gdH );
	}
	
	return 0;
}
