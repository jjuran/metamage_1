/*	=============
 *	PushButton.cc
 *	=============
 */

#include "Pedestal/PushButton.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// Nitrogen
#include "Nitrogen/Str.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline ControlRef NewMacPushButton( WindowRef owningWindow,
	                                           const Rect& boundsRect,
	                                           ConstStr255Param title,
	                                           const void* refCon )
	{
		return NewControl( owningWindow,
		                   &boundsRect,
		                   title,
		                   true,
		                   0,
		                   0,
		                   1,
		                   pushButProc,
		                   (long) refCon );
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
		SetControlBounds( itsControl, &bounds );
	}
	
	void PushButton::Activate( bool activating )
	{
		ControlPartCode code = activating ? kControlNoPart
		                                  : kControlInactivePart;
		
		HiliteControl( itsControl, code );
	}
	
}
