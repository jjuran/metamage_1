/*
	is_monochrome.cc
	----------------
*/

#include "mac_qd/is_monochrome.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/is_grayscale.hh"


namespace mac {
namespace qd  {

bool is_monochrome( const PixMap& pixmap )
{
	return pixmap.pixelSize == 1  ||  is_grayscale( pixmap );
}

}
}
