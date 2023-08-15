/*
	set_cursor_by_id.hh
	-------------------
*/

#ifndef MACQD_SETCURSORBYID_HH
#define MACQD_SETCURSORBYID_HH

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

// mac-sys-utils
#include "mac_sys/res_error.hh"


namespace mac {
namespace qd  {

inline
short set_cursor_by_id( short id )
{
	if ( CursHandle cursor_handle = GetCursor( id ) )
	{
		/*
			SetCursor() is safe to call at interrupt-time, so it can't
			possibly move memory and we don't need to lock the handle.
		*/
		
		SetCursor( *cursor_handle );
		
		return 0;
	}
	
	return mac::sys::res_error();
}

}
}

#endif
