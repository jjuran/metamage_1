/*	=================
 *	TrackScrollbar.cc
 *	=================
 */

#include "Pedestal/TrackScrollbar.hh"

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
		value = std::max( value, N::GetControlMinimum( control ) );
		value = std::min( value, N::GetControlMaximum( control ) );
		
		N::SetControlValue( control, value );
		
		return value;
	}
	
	static inline Scrollbar_UserData* GetUserDataFromScrollbar( ControlRef control )
	{
		Scrollbar_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData != NULL );
		
		return userData;
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
	
	namespace
	{
		
		void ScrollbarAction( ControlRef control, N::ControlPartCode part )
		{
			const short value = N::GetControlValue( control );
			
			const bool vertical = GetUserDataFromScrollbar( control )->vertical;
			
			const bool dragged = part == N::kControlIndicatorPart;
			
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
				
				if ( part == Mac::kControlPageUpPart  ||  part == Mac::kControlPageDownPart )
				{
					const short viewLength = vertical ? scroller->ViewHeight()
					                                  : scroller->ViewWidth ();
					
					const short pageDistance = viewLength - 1;
					
					scrollDistance = pageDistance;
				}
				
				if ( part == Mac::kControlUpButtonPart  ||  part == Mac::kControlPageUpPart )
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
		
	}
	
	
	void TrackScrollbar( ControlRef control, N::ControlPartCode part, Point point )
	{
		n::saved< N::Clip > savedClip;
		
		N::ClipRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
		
		const Scrollbar_UserData* userData = GetUserDataFromScrollbar( control );
		
		ASSERT( userData != NULL );
		
		// Get the current scrollbar value.
		const short oldValue = N::GetControlValue( control );
		
		switch ( part )
		{
			case kControlIndicatorPart:
				// The user clicked on the indicator
				
				if ( !MacFeatures::Has_Appearance() )
				{
					// Classic scrolling, handled specially.
					
					// Let the system track the drag...
					part = N::TrackControl( control, point );
					
					if ( part == N::kControlIndicatorPart )
					{
						// Drag was successful (i.e. within bounds).
						const short newValue = N::GetControlValue( control );
						
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
					if ( userData->afterHook  &&  N::GetControlValue( control ) != oldValue )
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

