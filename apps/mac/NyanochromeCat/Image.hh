/*
	Image.hh
	--------
*/

#ifndef IMAGE_HH
#define IMAGE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// nyancatlib
#include "nyancat/graphics.hh"


#ifdef __APPLE__

extern CGImageRef images[ nyancat::n_frames ];

void make_images_from_gworld( GWorldPtr gworld );

#endif

#endif
