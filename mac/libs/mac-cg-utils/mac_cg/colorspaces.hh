/*
	colorspaces.hh
	--------------
*/

#ifndef MACCG_COLORSPACES_HH
#define MACCG_COLORSPACES_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


namespace mac {
namespace cg  {

CGColorSpaceRef generic_or_device_gray();
CGColorSpaceRef generic_or_device_RGB();

CGColorSpaceRef create_inverted_grayscale( CGColorSpaceRef gray, int count );

}
}

#endif
