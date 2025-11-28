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

// Mac OS
#ifndef CGCOLORSPACE_H_
#ifndef __CGCOLORSPACE__
#include <CGColorSpace.h>
#endif
#endif


namespace mac {
namespace cg  {

CGColorSpaceRef generic_or_device_gray();
CGColorSpaceRef generic_or_device_RGB();

CGColorSpaceRef create_inverted_grayscale( CGColorSpaceRef gray, int count );

inline
CGColorSpaceRef create_inverted_grayscale( int count )
{
	return create_inverted_grayscale( generic_or_device_gray(), count );
}

CGColorSpaceRef create_RGB_palette( CGColorSpaceRef  rgb,
                                    const UInt16*    colors,
                                    int              count );

inline
CGColorSpaceRef create_RGB_palette( const UInt16*  colors,
                                    int            count )
{
	return create_RGB_palette( generic_or_device_RGB(), colors, count );
}

}
}

#endif
