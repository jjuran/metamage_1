/*	=================
 *	TrackScrollbar.cc
 *	=================
 */

#include "Pedestal/TrackScrollbar.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/has/Appearance.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/scoped_clipRect.hh"

// Debug
#include "debug/assert.hh"

// Pedestal
#include "Pedestal/Scroller_beta.hh"


namespace Pedestal
{
	
	static inline
	short
	min( short a, short b )
	{
		return b < a ? b : a;
	}
	
	static inline
	short
	max( short a, short b )
	{
		return a > b ? a : b;
	}
	
	static short SetClippedControlValue( ControlRef control, short value )
	{
		value = max( value, GetControlMinimum( control ) );
		value = min( value, GetControlMaximum( control ) );
		
		SetControlValue( control, value );
		
		return value;
	}
	
	static inline Scrollbar_UserData* GetUserDataFromScrollbar( ControlRef control )
	{
		void* userData = (void*) GetControlReference( control );
		
		ASSERT( userData != NULL );
		
		return (Scrollbar_UserData*) userData;
	}
	
	static inline ScrollerAPI* RecoverScrollerFromScrollbar( ControlRef control )
	{
		Scrollbar_UserData* userData = GetUserDataFromScrollbar( control );
		
		ASSERT( userData != NULL );
		
		ASSERT( userData->fetchHook != NULL );
		
		ScrollerAPI* scroller = userData->fetchHook( control );
		
		ASSERT( scroller != NULL );
		
		return scroller;
	}
	
	static inline void SetScrollOffset( ScrollerAPI* scroller, bool vertical, short value )
	{
		ASSERT( scroller != NULL );
		
		if ( vertical )
		{
			scroller->SetVOffset( value );
		}
		else
		{
			scroller->SetHOffset( value );
		}
	}
	
	static
	pascal void Scroll_Action( ControlRef control, ControlPartCode part )
	{
		const short value = GetControlValue( control );
		
		const bool vertical = GetUserDataFromScrollbar( control )->vertical;
		
		const bool dragged = part == kControlIndicatorPart;
		
		ScrollerAPI* scroller = RecoverScrollerFromScrollbar( control );
		
		short oldValue = value;
		short newValue = value;
		
		if ( dragged )
		{
			oldValue = vertical ? scroller->GetVOffset()
			                    : scroller->GetHOffset();
			
			newValue = value;
		}
		else
		{
			oldValue = value;
			
			short scrollDistance = 1;
			
			if ( part == kControlPageUpPart  ||  part == kControlPageDownPart )
			{
				const short viewLength = vertical ? scroller->ViewHeight()
				                                  : scroller->ViewWidth ();
				
				const short pageDistance = viewLength - 1;
				
				scrollDistance = pageDistance;
			}
			
			if ( part == kControlUpButtonPart  ||  part == kControlPageUpPart )
			{
				scrollDistance = -scrollDistance;
			}
			
			newValue = SetClippedControlValue( control, value + scrollDistance );
		}
		
		if ( newValue != oldValue )
		{
			SetScrollOffset( scroller, vertical, newValue );
		}
	}
	
	DEFINE_UPP( ControlAction, Scroll_Action )
	
	void TrackScrollbar( ControlRef control, ControlPartCode part, Point point )
	{
		using namespace mac::qd;
		
		static RgnHandle tmp = NewRgn();
		
		scoped_clipRect clipRect( get_portRect( thePort() ), tmp );
		
		const Scrollbar_UserData* userData = GetUserDataFromScrollbar( control );
		
		ASSERT( userData != NULL );
		
		// Get the current scrollbar value.
		const short oldValue = GetControlValue( control );
		
		switch ( part )
		{
			case kControlIndicatorPart:
				// The user clicked on the indicator
				
				if ( ! mac::sys::has_Appearance() )
				{
					// Classic scrolling, handled specially.
					
					// Let the system track the drag...
					part = TrackControl( control, point, NULL );
					
					if ( part == kControlIndicatorPart )
					{
						// Drag was successful (i.e. within bounds).
						const short newValue = GetControlValue( control );
						
						if ( newValue != oldValue )
						{
							ScrollerAPI* scroller = RecoverScrollerFromScrollbar( control );
							
							ASSERT( scroller != NULL );
							
							SetScrollOffset( scroller, userData->vertical, newValue );
						}
					}
					
					// Break here for classic thumb-scrolling (whether successful or not).
					break;
				}
				// else fall through for live feedback scrolling
			case kControlUpButtonPart:
			case kControlDownButtonPart:
			case kControlPageUpPart:
			case kControlPageDownPart:
				part = TrackControl( control, point, UPP_ARG( Scroll_Action ) );
				
				if ( part  &&  userData->afterHook )
				{
					if ( GetControlValue( control ) != oldValue )
					{
						userData->afterHook( control );
					}
				}
				
				break;
			
			default:
				break;
		}
	}
	
}
