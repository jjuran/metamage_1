/*
	scrolling.cc
	------------
*/

#include "scrolling.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif


static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static inline
short max( short a, short b )
{
	return a > b ? a : b;
}

pascal
void scrollbar_actionProc( ControlRef scrollbar, short part )
{
	const UInt32 refcon = scrollbar[0]->contrlRfCon;
	
	const scroll_action_rec& action = *(const scroll_action_rec*) refcon;
	
	const short initial_value = GetControlValue( scrollbar );
	
	short value = initial_value;
	
	/*
		kControlUpButtonPart   = 20
		kControlDownButtonPart = 21
		kControlPageUpPart     = 22
		kControlPageDownPart   = 23
	*/
	
	if ( (part & ~3) == 20 )
	{
		// Extract either action.step or action.page as the delta.
		
		const short delta = *(short*) (refcon + (part & 0x6));
		
		if ( part & 1 )
		{
			value = min( value + delta, GetControlMaximum( scrollbar ) );
		}
		else
		{
			value = max( value - delta, 0 );
		}
		
		if ( value != initial_value )
		{
			SetControlValue( scrollbar, value );
			
			action.scroll_to( action, value );
		}
	}
}
