/*	==========
 *	Control.cc
 *	==========
 */

#include "Pedestal/Control.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
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
	
}

