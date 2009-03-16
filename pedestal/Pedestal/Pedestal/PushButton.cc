/*	=============
 *	PushButton.cc
 *	=============
 */

#include "Pedestal/PushButton.hh"

// Universal Interfaces
#ifndef __FONTS__
#include <Fonts.h>
#endif

// Nitrogen
#include "Nitrogen/ControlDefinitions.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	
	inline ControlRef NewMacPushButton( WindowRef owningWindow,
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
	
	static Rect CalcCenteredButtonRect( const Rect& frame, ConstStr255Param title )
	{
		return NX::CenteredRect( frame, CalcButtonSize( title ) );
	}
	
	
	void PushButton::Install( const Rect& bounds )
	{
		N::Str255 title = Title();
		
		itsControl = NewMacPushButton( N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
		                               CalcCenteredButtonRect( bounds, title ),
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
		N::SetControlBounds( itsControl, CalcCenteredButtonRect( bounds, Title() ) );
	}
	
	void PushButton::Activate( bool activating )
	{
		N::ControlPartCode code = activating ? N::kControlNoPart
		                                     : N::kControlInactivePart;
		
		N::HiliteControl( itsControl, code );
	}
	
}

