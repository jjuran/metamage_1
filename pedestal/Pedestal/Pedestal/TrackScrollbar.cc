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

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/Appearance.hh"

// Pedestal
#include "Pedestal/Scroller_beta.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static short SetClippedControlValue( ControlRef control, short value )
	{
		value = std::max( value, GetControlMinimum( control ) );
		value = std::min( value, GetControlMaximum( control ) );
		
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
	
	// Template parameters must have extern linkage
	void ScrollbarAction( ControlRef control, N::ControlPartCode part );
	
	void ScrollbarAction( ControlRef control, N::ControlPartCode part )
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
	
	
	void TrackScrollbar( ControlRef control, N::ControlPartCode part, Point point )
	{
		n::saved< N::Clip > savedClip;
		
		N::ClipRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
		
		const Scrollbar_UserData* userData = GetUserDataFromScrollbar( control );
		
		ASSERT( userData != NULL );
		
		// Get the current scrollbar value.
		const short oldValue = GetControlValue( control );
		
		switch ( part )
		{
			case kControlIndicatorPart:
				// The user clicked on the indicator
				
				if ( !MacFeatures::Has_Appearance() )
				{
					// Classic scrolling, handled specially.
					
					// Let the system track the drag...
					part = N::TrackControl( control, point );
					
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
				if (( part = N::TrackControl< ScrollbarAction >( control, point ) ))
				{
					if ( userData->afterHook  &&  GetControlValue( control ) != oldValue )
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
