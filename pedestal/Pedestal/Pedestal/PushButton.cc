/*	=============
 *	PushButton.cc
 *	=============
 */

#include "Pedestal/PushButton.hh"

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// Nitrogen
#include "Mac/Controls/Types/ControlPartCode.hh"

#include "Nitrogen/Controls.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline ControlRef NewMacPushButton( WindowRef owningWindow,
	                                           const Rect& boundsRect,
	                                           ConstStr255Param title,
	                                           const void* refCon )
	{
		return N::NewControl( owningWindow,
		                      boundsRect,
		                      title,
		                      true,
		                      0,
		                      0,
		                      1,
		                      Mac::pushButProc,
		                      refCon );
	}
	
	
	void PushButton::Install( const Rect& bounds )
	{
		N::Str255 title = Title();
		
		itsControl = NewMacPushButton( mac::qd::thePort_window(),
		                               bounds,
		                               title,
		                               RefCon() );
		
		HiliteControl( itsControl, kControlInactivePart );
	}
	
	void PushButton::Uninstall()
	{
		::DisposeControl( itsControl );
		
		itsControl = NULL;
	}
	
	void PushButton::SetBounds( const Rect& bounds )
	{
		N::SetControlBounds( itsControl, bounds );
	}
	
	void PushButton::Activate( bool activating )
	{
		N::ControlPartCode code = activating ? N::kControlNoPart
		                                     : N::kControlInactivePart;
		
		N::HiliteControl( itsControl, code );
	}
	
}
