/*	==========
 *	Control.cc
 *	==========
 */

#include "Pedestal/Control.hh"

// Universal Interfaces
#include <ControlDefinitions.h>
#include <Fonts.h>


namespace Pedestal {
	
	namespace N = Nitrogen;
	
	bool TrackControl( const FindControl_Result& param, Point startPoint )
	{
		// Did we actually hit a control?
		if ( param.part != kControlNoPart )
		{
			// If the control has an action routine, it's not a Pedestal control.
			// It might (for example) be a List-Manager-owned scrollbar, 
			// which we trigger through LClick().
			// Return false to indicate that we didn't handle the event.
			if ( N::GetControlAction( param.control ) == NULL )
			{
				Control_Hooks* controlHooks = N::GetControlReference( param.control );
				
				if ( controlHooks != NULL )
				{
					ControlTracker trackControl = controlHooks->trackHook;
					if ( trackControl != NULL )
					{
						trackControl( param.control, param.part, startPoint );
					}
					return true;
				}
			}
		}
		return false;
	}
	
	void Control_InvariantDetails::Move( object_type object, short h, short v )
	{
		N::MoveControl( object, h, v );
	}
	
	void Control_InvariantDetails::Resize( object_type object, short width, short height )
	{
		N::SizeControl( object, width, height );
	}
	
	void Control_InvariantDetails::SetBounds( object_type object, const Rect& bounds )
	{
		N::HideControl( object );
		
		Move( object, bounds.left, bounds.top );
		Resize( object, bounds.right - bounds.left, bounds.bottom - bounds.top );
		
		N::ShowControl( object );
	}
	
	void Control_DefaultDetails::TrackControl( object_type object, ControlPartCode part, Point point )
	{
		if ( part != kControlNoPart )
		{
			part = N::TrackControl( object, point );
			
			if ( part != kControlNoPart )
			{
				
			}
		}
	}
	
	
	Control::Control( const Rect& bounds )
	{
	}
	
	/*
	void
	Control::SetMinimumClip( short min )
	{
		if ( macControl )
		{
			if ( N::GetControlValue( macControl ) < min )
			{
				if ( N::GetControlMaximum( macControl ) < min )
				{
					N::SetControlMaximum( macControl, min );
				}
				N::SetControlValue( macControl, min );
			}
			N::SetControlMinimum( macControl, min );
		}
	}
	
	void
	Control::SetMaximumClip( short max )
	{
		if ( macControl )
		{
			if ( N::GetControlValue( macControl ) > max )
			{
				if ( N::GetControlMinimum( macControl ) > max )
				{
					N::SetControlMinimum( macControl, max );
				}
				N::SetControlValue( macControl, max );
			}
			N::SetControlMaximum( macControl, max );
		}
	}
	
	void
	Control::SetValuePin( short val )
	{
		if ( macControl )
		{
			short min = N::GetControlMinimum( macControl );
			short max = N::GetControlMaximum( macControl );
			if ( val > max )
			{
				val = max;
			}
			else if ( val < min )
			{
				val = min;
			}
			N::SetControlValue( macControl, val );
		}
	}
	*/
	
	void
	Control::SetValueStretch( short val )
	{
		if ( macControl )
		{
			if ( val > N::GetControlMaximum( macControl ) )
			{
				N::SetControlMaximum( macControl, val );
			}
			else if ( val < N::GetControlMinimum( macControl ) )
			{
				N::SetControlMinimum( macControl, val );
			}
			N::SetControlValue( macControl, val );
		}
	}
	
	void SetValueStretch( ControlRef control, short value )
	{
		if ( value > N::GetControlMaximum( control ) )
		{
			N::SetControlMaximum( control, value );
		}
		else if ( value < N::GetControlMinimum( control ) )
		{
			N::SetControlMinimum( control, value );
		}
		N::SetControlValue( control, value );
	}
	
	
	void
	Control::Activate()
	{
		if ( macControl )
		{
			//::ShowControl(macControl);
		}
	}
	
	void
	Control::Deactivate()
	{
		if ( macControl )
		{
			//::HideControl(macControl);
		}
	}
	
	void
	Control::Resize( short width, short height )
	{
		
	}
	
}

