/*	============
 *	Scrollbar.cc
 *	============
 */

#include "Pedestal/Scrollbar.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/has/Appearance.hh"

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// Debug
#include "debug/assert.hh"


#pragma exceptions off


namespace Pedestal
{
	
	static inline
	short
	max( short a, short b )
	{
		return a > b ? a : b;
	}
	
	static inline
	short GetControlProcIDForScrollbar()
	{
		const bool liveScrolling = mac::sys::has_Appearance();
		
		return liveScrolling ? (short) kControlScrollBarLiveProc
		                     : (short) scrollBarProc;
	}
	
	
	void Scrollbar::Activate( bool activating )
	{
		if ( itsControl == NULL )
		{
			return;
		}
		
		if ( activating )
		{
			//ShowControl( itsControl );
			HiliteControl( itsControl, kControlNoPart );
		}
		else
		{
			//HideControl( itsControl );
			HiliteControl( itsControl, kControlInactivePart );
		}
	}
	
	void Scrollbar::Create( const Rect& bounds, const void* refCon )
	{
		ASSERT( itsControl == NULL );
		
		const short procID = GetControlProcIDForScrollbar();
		
		itsControl = NewControl( mac::qd::thePort_window(),
		                         &bounds,
		                         "\p",
		                         true,
		                         0,
		                         0,
		                         0,
		                         procID,
		                         (long) refCon );
	}
	
	void Scrollbar::UpdateBounds( const Rect& bounds )
	{
		ASSERT( itsControl != NULL );
		
		Rect currentBounds;
		GetControlBounds( itsControl, &currentBounds );
		
		if ( memcmp( &currentBounds, &bounds, sizeof (Rect) ) != 0 )
		{
			HideControl( itsControl );
			
			MoveControl( itsControl, bounds.left, bounds.top );
			SizeControl( itsControl, bounds.right - bounds.left, bounds.bottom - bounds.top );
			
			ShowControl( itsControl );
		}
	}
	
	void Scrollbar::Clear()
	{
		if ( itsControl )
		{
			DisposeControl( itsControl );
			
			itsControl = NULL;
		}
	}
	
	void Scrollbar::SetValue( short value )
	{
		if ( itsControl )
		{
			SetControlValue( itsControl, value );
		}
	}
	
	static inline bool Has_SetControlViewSize()
	{
		return   TARGET_API_MAC_CARBON ? true
		       : TARGET_CPU_68K        ? false
		       :                         &::SetControlViewSize != NULL;
	}
	
	void Scrollbar::Adjust( int clientLength, int offset, int viewLength )
	{
		if ( itsControl )
		{
			short max_offset = max( clientLength - viewLength, offset );
			
			SetControlMaximum( itsControl, max_offset );
			
			SetValue( offset );
			
			if ( Has_SetControlViewSize() )
			{
				::SetControlViewSize( itsControl, viewLength );
			}
		}
	}
	
}
