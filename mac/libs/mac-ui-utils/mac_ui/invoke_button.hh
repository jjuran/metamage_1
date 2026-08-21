/*
	invoke_button.hh
	----------------
*/

#ifndef MACUI_INVOKEBUTTON_HH
#define MACUI_INVOKEBUTTON_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif

// mac-glue-utils
#include "mac_glue/OSUtils.hh"


namespace mac {
namespace ui  {
	
#if ! __LP64__
	
	inline
	void HiliteControl_immediate( ControlRef control, short part )
	{
		HiliteControl( control, part );
		
		if ( TARGET_API_MAC_CARBON )
		{
			QDFlushPortBuffer( GetQDGlobalsThePort(), NULL );
		}
	}
	
	inline
	void invoke_button_inline( ControlRef button )
	{
		HiliteControl_immediate( button, kControlButtonPart );
		
		mac::glue::delay( 8 );
		
		HiliteControl_immediate( button, kControlNoPart );
	}
	
#endif
	
}
}

#endif
