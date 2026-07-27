/*
	CreateImage.hh
	--------------
*/

#ifndef CREATEIMAGE_HH
#define CREATEIMAGE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


CGImageRef CreateImageFromURL( CFURLRef url );

#endif
