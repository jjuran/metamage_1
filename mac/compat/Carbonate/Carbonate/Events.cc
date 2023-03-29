// Carbonate/Events.cc


#ifndef __EVENTS__
#include <Events.h>
#endif

#if !TARGET_API_MAC_CARBON

// These functions are always declared in the headers and are always extern.


/*
Event modifiers in low memory:

	0x017B
	0	Shift
	1	Alpha Lock
	2	Option
	3	Control
	4
	5
	6
	7	Function
	
	0x017A
	0
	1
	2
	3
	4
	5
	6
	7	Command

Key modifiers:
	0	(activate bit in EventModifiers)
	1
	2
	3
	4
	5
	6
	7	(button bit in EventModifiers)
	
	8	Command
	9	Shift
	10	Alpha Lock
	11	Option
	12	Control
	13	right Shift
	14	right Option
	15	right Control
	
	16
	17	Function
	...


*/

#if !ACCESSOR_CALLS_ARE_FUNCTIONS

pascal void GetGlobalMouse( Point* globalMouse )
{
	const EventMask kNoEvents = 0;
	
	EventRecord event;
	
	(void) OSEventAvail( kNoEvents, &event );
	
	*globalMouse = event.where;
}

#endif

pascal UInt32 GetCurrentKeyModifiers()
{
	// Credit to Lawrence D'Oliveiro for the low memory tip, from PsyScript.
	UInt32 lowMemModifiers = *reinterpret_cast< UInt16* >( 0x017A );
	
	UInt32 keyModifiers = (lowMemModifiers & 0x0080) << 10  // Function
	                    | (lowMemModifiers & 0x007F) << 9   // Shift/Alpha/Option/Control
	                    | (lowMemModifiers & 0x8000) >> 7;  // Command
	
	return keyModifiers;
}

pascal Boolean CheckEventQueueForUserCancel()
{
	bool canceled = false;
	
	QHdrPtr eventQueueHeader = ::GetEvQHdr();
	
	for ( EvQElPtr eventQueuePtr =  reinterpret_cast< EvQElPtr >( eventQueueHeader->qHead );
		  eventQueuePtr          != NULL  &&  !canceled;
		  eventQueuePtr          =  reinterpret_cast< EvQElPtr >( eventQueuePtr->qLink ) )
	{
		if ( eventQueuePtr->evtQWhat == keyDown )
		{
			bool command = eventQueuePtr->evtQModifiers & cmdKey;
			char keyChar = eventQueuePtr->evtQMessage;
			
			if ( command && keyChar == '.' )
			{
				canceled = true;
			}
			else if ( keyChar == kEscapeCharCode )
			{
				char keyCode = eventQueuePtr->evtQMessage >> 8;
				
				if ( keyCode == 0x35 )
				{
					canceled = true;
				}
			}
		}
	}
	
	if ( canceled )
	{
		// Remove mouse and key events
	}
	
	return canceled;
}

#endif
