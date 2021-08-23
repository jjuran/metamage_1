/*	============
 *	Scrollbar.cc
 *	============
 */

#include "Pedestal/Scrollbar.hh"

// Standard C++
#include <algorithm>

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/Appearance.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline N::ControlProcID GetControlProcIDForScrollbar()
	{
		const bool liveScrolling = MacFeatures::Has_Appearance();
		
		return liveScrolling ? Mac::kControlScrollBarLiveProc
		                     : Mac::scrollBarProc;
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
		
		const N::ControlProcID procID = GetControlProcIDForScrollbar();
		
		itsControl = N::NewControl( mac::qd::thePort_window(),
		                            bounds,
		                            "\p",
		                            true,
		                            0,
		                            0,
		                            0,
		                            procID,
		                            refCon );
	}
	
	void Scrollbar::UpdateBounds( const Rect& bounds )
	{
		using namespace nucleus::operators;
		
		ASSERT( itsControl != NULL );
		
		if ( N::GetControlBounds( itsControl ) != bounds )
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
			short max_offset = std::max( clientLength - viewLength, offset );
			
			SetControlMaximum( itsControl, max_offset );
			
			SetValue( offset );
			
			if ( Has_SetControlViewSize() )
			{
				::SetControlViewSize( itsControl, viewLength );
			}
		}
	}
	
}
