/*	=============
 *	PushButton.cc
 *	=============
 */

#include "Pedestal/PushButton.hh"

// Universal Interfaces
#include <Fonts.h>

// Nitrogen
#include "Nitrogen/ControlDefinitions.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	
	void PushButton::TrackControl( ControlRef control, ControlPartCode part, Point point )
	{
		if ( part != kControlNoPart )
		{
			part = N::ControlPartCode( ::TrackControl( control, point, NULL ) );
			
			if ( part != kControlNoPart )
			{
				Control_Hooks* controlHooks = reinterpret_cast< Control_Hooks* >( GetControlReference( control ) );
				
				// controlHooks shouldn't be NULL if we got here
				PushButton* button = reinterpret_cast< PushButton* >( controlHooks->data );
				
				if ( button != NULL )
				{
					ButtonAction action = button->action;
					
					if (action != NULL )
					{
						action( button->refCon );
					}
				}
			}
		}
	}
	
	inline ControlRef NewMacPushButton( WindowRef owningWindow,
	                                    const Rect& boundsRect,
	                                    ConstStr255Param title,
	                                    //Control* refCon
	                                    void* refCon )
	{
		return N::NewControl( owningWindow,
		                      boundsRect,
		                      title,
		                      true,
		                      0,
		                      0,
		                      1,
		                      pushButProc,
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
	
	PushButton::PushButton( const Rect& bounds, const Initializer& init )
	:
		Control( bounds      ),
		action ( init.action ),
		refCon ( init.refCon )
	{
		controlHooks.data = this;
		controlHooks.trackHook = TrackControl;
		
		macControl = NewMacPushButton( N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
		                               CalcCenteredButtonRect( bounds, init.title ),
		                               init.title,
		                               &controlHooks );
	}
	
}

