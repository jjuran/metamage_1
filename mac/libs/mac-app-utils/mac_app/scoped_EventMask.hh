/*
	scoped_EventMask.hh
	-------------------
*/

#ifndef MACAPP_SCOPEDEVENTMASK_HH
#define MACAPP_SCOPEDEVENTMASK_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// iota
#include "iota/class.hh"


#define SysEvtMask  (*(UInt16*) 0x0144)


namespace mac {
namespace app {

class scoped_EventMask
{
	NON_COPYABLE( scoped_EventMask )
	NO_NEW_DELETE
	
	private:
		UInt16 saved_eventMask;
	
	public:
		explicit scoped_EventMask( UInt16 eventMask )
		{
			if ( TARGET_CPU_68K )
			{
				saved_eventMask = SysEvtMask;
			}
			
			SetEventMask( eventMask );
		}
		
		~scoped_EventMask()
		{
			if ( TARGET_CPU_68K )
			{
				SysEvtMask = saved_eventMask;
			}
		}
};

}
}

#undef SysEvtMask

#endif
