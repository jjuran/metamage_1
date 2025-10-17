/*
	wait_next_event.hh
	------------------
*/

#ifndef MACAPP_WAITNEXTEVENT_HH
#define MACAPP_WAITNEXTEVENT_HH


struct EventRecord;

typedef unsigned char   Boolean;
typedef unsigned short  EventMask;

namespace mac {
namespace app {
	
	Boolean wait_next_event( EventMask event_mask, EventRecord& event );
	
	inline
	Boolean wait_next_event( EventRecord& event )
	{
		enum
		{
			everyEvent = 0xFFFF,
		};
		
		return wait_next_event( everyEvent, event );
	}
	
}
}

#endif
