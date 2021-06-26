/*
	is_port_visrgn_empty.hh
	-----------------------
*/

#ifndef MACQD_ISPORTVISRGNEMPTY_HH
#define MACQD_ISPORTVISRGNEMPTY_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace mac {
namespace qd  {
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	inline
	bool is_port_visrgn_empty( GrafPtr port )
	{
		return EmptyRgn( port->visRgn );
	}
	
	inline
	bool is_port_visrgn_empty( CGrafPtr port )
	{
		return EmptyRgn( port->visRgn );
	}
	
#else
	
	inline
	bool is_port_visrgn_empty( GrafPtr port )
	{
		return IsPortVisibleRegionEmpty( port );
	}
	
	inline
	bool is_port_visrgn_empty( WindowRef window )
	{
		return is_port_visrgn_empty( GetWindowPort( window ) );
	}
	
#endif
	
}
}

#endif
