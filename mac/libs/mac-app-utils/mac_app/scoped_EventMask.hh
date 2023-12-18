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


#define SysEvtMask  (*(UInt16*) 0x0144)


namespace mac {
namespace app {

class scoped_EventMask
{
	private:
		UInt16 saved_eventMask;
		
		// non-copyable
		scoped_EventMask           ( const scoped_EventMask& );
		scoped_EventMask& operator=( const scoped_EventMask& );
	
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
