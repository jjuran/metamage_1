/*
	icongen.hh
	----------
*/

#ifndef ICONGEN_ICONGEN_HH
#define ICONGEN_ICONGEN_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


typedef void (*drawer)( CGContextRef c, size_t width, size_t height );

int icongen_main( int argc, char** argv, drawer draw );


#endif
