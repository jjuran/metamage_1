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
	
#elif ! __LP64__
	
	inline
	bool is_port_visrgn_empty( GrafPtr port )
	{
		/*
			Non-Carbon calls to IsPortVisibleRegionEmpty() are statically
			linked to the Carbonate library which defines it inline.
			
			Carbon didn't add IsPortVisibleRegionEmpty() until CarbonLib 1.3,
			so we have to check before calling it.  If it's not present,
			be pessimistic but safe and return false.
		*/
		
		if ( TARGET_API_MAC_CARBON  &&  &IsPortVisibleRegionEmpty == NULL )
		{
			return false;
		}
		
		return IsPortVisibleRegionEmpty( port );
	}
	
	inline
	bool is_port_visrgn_empty( WindowRef window )
	{
		return is_port_visrgn_empty( GetWindowPort( window ) );
	}
	
#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
	
}
}

#endif
