/*	==========
 *	AutoKey.cc
 *	==========
 */

#include "Pedestal/AutoKey.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif


namespace Pedestal
{
	
	static EventRecord gLastTextEntryKeyDownEvent;
	
	static unsigned gTextEntryKeyCount = 0;
	
	
	static inline bool AutoKeyRequiresThreeStrikes()
	{
		// False in Mac HIG
		return true;
	}
	
	
	void ResetAutoKeyCount()
	{
		gTextEntryKeyCount = 0;
	}
	
	bool Try_IgnoreAutoKey( const EventRecord& event )
	{
		if ( AutoKeyRequiresThreeStrikes()  &&  event.what == autoKey  &&  gTextEntryKeyCount < 3 )
		{
			// Suppress auto-key until after three consecutive key-downs
			return true;
		}
		
		if ( event.what == keyDown )
		{
			if (    event.message   == gLastTextEntryKeyDownEvent.message
			     && event.modifiers == gLastTextEntryKeyDownEvent.modifiers )
			{
				++gTextEntryKeyCount;
			}
			else
			{
				gTextEntryKeyCount = 1;
			}
		}
		
		gLastTextEntryKeyDownEvent = event;
		
		return false;
	}
	
}

