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
#ifndef __MC68K__
inline
#endif
void StillDown_payload()
{
	if ( animation_timer.pulse() )
	{
		GrafPtr port;
		
		GetPort( &port );
		SetPortWindowPort( main_window );
		
		prepare_next_frame();
		blit_window( main_window );
		
		SetPort( port );
	}
}

#if CALL_NOT_IN_CARBON

#ifdef __MC68K__

/*
	Our StillDown() patch gets called with a different value in A5
	than when we call DragWindow(), so we have to save/set/restore it.
	
	(This hasn't been an issue with TrackGoAway().)
*/

static
asm
pascal Boolean StillDown_patch()
{
	MOVE.L   A5,-(SP)
	MOVEA.L  0x0904,A5           // CurrentA5
	
	JSR      StillDown_payload
	
	MOVEA.L  old_StillDown,A0    // read A5-based global with our A5
	MOVEA.L  (SP)+,A5
	
	JMP      (A0)
}

#else  // #ifdef __MC68K__

static
pascal Boolean StillDown_patch()
{
	StillDown_payload();
	
	return InvokeStillDownPatchUPP( old_StillDown );
}

DEFINE_UPP( StillDownPatch, StillDown_patch )

#endif  // #ifdef __MC68K__

using mac::sys::get_trap_address;
using mac::sys::set_trap_address;

typedef UniversalProcPtr UPP;

Boolean TrackGoAway_magic( WindowRef window, Point pt )
{
	if ( animation_timer.paused() )
	{
		return TrackGoAway( window, pt );
	}
	
	old_StillDown = (StillDownPatchUPP) get_trap_address( _StillDown );
	
	set_trap_address( (UPP) UPP_ARG( StillDown_patch ), _StillDown );
	
	const Boolean result = TrackGoAway( window, pt );
	
	set_trap_address( (UPP) old_StillDown, _StillDown );
	
	return result;
}

#endif  // #if CALL_NOT_IN_CARBON
