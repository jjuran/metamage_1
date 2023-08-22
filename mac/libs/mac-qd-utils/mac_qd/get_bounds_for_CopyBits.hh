/*
	get_bounds_for_CopyBits.hh
	--------------------------
*/

#ifndef MACQD_GETBOUNDSFORCOPYBITS_HH
#define MACQD_GETBOUNDSFORCOPYBITS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"


#if ! __LP64__

namespace mac {
namespace qd  {

inline
const Rect& get_bounds_for_CopyBits( const BitMap& bitmap )
{
	return bitmap.bounds;
}

inline
const Rect& get_bounds_for_CopyBits( const PixMap& pixmap )
{
	return pixmap.bounds;
}

#if OPAQUE_TOOLBOX_STRUCTS
typedef Rect portRect_result_type;
#else
typedef const Rect& portRect_result_type;
#endif

inline
portRect_result_type get_bounds_for_CopyBits( CGrafPtr port )
{
	return get_portRect( port );
}

inline
portRect_result_type get_bounds_for_CopyBits( WindowRef window )
{
	return get_portRect( window );
}

}
}

#endif  // #if ! __LP64__

#endif
