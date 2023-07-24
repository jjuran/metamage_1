/*
	Magic.cc
	--------
*/

#include "Magic.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#if ! TARGET_API_MAC_CARBON
#include "mac_sys/trap_address.hh"
#endif

// NyanochromeCat
#include "Offscreen.hh"
#include "PatchUPPs.hh"
#include "Timer.hh"
#include "Window.hh"


enum
{
	_StillDown = 0xA973,
};

StillDownPatchUPP old_StillDown;

static
pascal void StillDown_payload()
{
	if ( animation_timer.pulse() )
	{
		GrafPtr port;
		
		GetPort( &port );
		SetPortWindowPort( main_window );
		
		prepare_next_frame();
		
		draw_window( main_window );
		
		SetPort( port );
	}
}

#if CALL_NOT_IN_CARBON

using mac::sys::get_trap_address;
using mac::sys::set_trap_address;

static
pascal Boolean StillDown_patch()
{
	StillDown_payload();
	
	return InvokeStillDownPatchUPP( old_StillDown );
}

DEFINE_UPP( StillDownPatch, StillDown_patch )

Boolean TrackGoAway_magic( WindowRef window, Point pt )
{
	if ( animation_timer.paused() )
	{
		return TrackGoAway( window, pt );
	}
	
	old_StillDown = (StillDownPatchUPP) get_trap_address( _StillDown );
	
	set_trap_address( (UniversalProcPtr) UPP_ARG( StillDown_patch ), _StillDown );
	
	const Boolean result = TrackGoAway( window, pt );
	
	set_trap_address( (UniversalProcPtr) old_StillDown, _StillDown );
	
	return result;
}

#endif  // #if CALL_NOT_IN_CARBON
