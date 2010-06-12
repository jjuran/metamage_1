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

// Nitrogen
#include "Nitrogen/ControlDefinitions.hh"
#include "Nitrogen/MacWindows.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline ControlRef NewMacPushButton( WindowRef owningWindow,
	                                           const Rect& boundsRect,
	                                           ConstStr255Param title,
	                                           void* refCon )
	{
		return N::NewControl( owningWindow,
		                      boundsRect,
		                      title,
		                      true,
		                      0,
		                      0,
		                      1,
		                      N::pushButProc,
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
		
		itsControl = NewMacPushButton( N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
		                               bounds,
		                               title,
		                               RefCon() );
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

