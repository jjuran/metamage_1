/*
	icon.hh
	-------
*/

#ifndef ICON_HH
#define ICON_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


void draw_upscaled_icon( CGContextRef c, size_t width, size_t height );

inline
void draw_the_icon( CGContextRef c, size_t width, size_t height )
{
	draw_upscaled_icon( c, width, height );
}


#endif
