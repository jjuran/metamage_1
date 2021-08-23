/*	=============
 *	PushButton.cc
 *	=============
 */

#include "Pedestal/PushButton.hh"

// Standard C++
#include <algorithm>

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// Nitrogen
#include "Mac/Controls/Types/ControlPartCode.hh"

#include "Nitrogen/Controls.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Quickdraw.hh"


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
	
	
	static Point CalcButtonSize( ConstStr255Param title )
	{
		int width = 1 + 8 + ::StringWidth( title ) + 8 + 1;
		
		width = std::max( width, 58 );
		
		return N::SetPt( width, 20 );
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
