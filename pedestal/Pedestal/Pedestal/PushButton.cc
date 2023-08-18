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


#pragma exceptions off


namespace Pedestal
{
	
	static inline ControlRef NewMacPushButton( WindowRef owningWindow,
	                                           const Rect& boundsRect )
	{
		return NewControl( owningWindow,
		                   &boundsRect,
		                   "\p",
		                   true,
		                   0,
		                   0,
		                   1,
		                   pushButProc,
		                   0 );
	}
	
	
	void PushButton::Install( const Rect& bounds )
	{
		itsControl = NewMacPushButton( mac::qd::thePort_window(),
		                               bounds );
		
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
