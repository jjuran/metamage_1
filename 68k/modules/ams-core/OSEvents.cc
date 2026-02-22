/*
	OSEvents.cc
	-----------
*/

#include "OSEvents.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// ams-common
#include "callouts.hh"

// ams-core
#include "reactor-core.hh"
#include "throttle.hh"


enum
{
	kOSEventWaiting = 0x0200,
};

Ptr  SysEvtBuf  : 0x0146;
QHdr EventQueue : 0x014A;
short SysEvtCnt : 0x0154;

Byte   MBState : 0x0172;
UInt16 KeyMods : 0x017A;
Point  Mouse   : 0x0830;


struct OSEvQEl
{
	UInt32  flags;
	EvQEl   qElem;
};

static
EvQEl* dequeue_event( QElemPtr it )
{
	Dequeue( it, &EventQueue );
	
	EvQEl* el = (EvQEl*) it;
	
	el->qType = 0;
	
	return el;
}

static
EvQEl* find_empty_slot()
{
	OSEvQEl* it = (OSEvQEl*) SysEvtBuf;
	
	short n = SysEvtCnt;  // n - 1, actually
	
	do
	{
		if ( it->qElem.qType == 0 )
		{
			return &it->qElem;
		}
		
		++it;
	}
	while ( --n >= 0 );
	
	return dequeue_event( EventQueue.qHead );
}

static inline
UInt16 get_modifiers()
{
	return (KeyMods << 1 | KeyMods >> 15) << 8 | MBState;
}

short PostEvent_patch( short what : __A0, long msg : __D0 )
{
	// TODO:  Reject events not enabled in the system event mask.
	
	EvQEl* qElem = find_empty_slot();
	
	qElem->qType = evType;
	
	qElem->evtQWhat    = what;
	qElem->evtQMessage = msg;
	
	qElem->evtQWhen      = get_Ticks();
	qElem->evtQWhere     = Mouse;
	qElem->evtQModifiers = get_modifiers();
	
	Enqueue( (QElemPtr) qElem, &EventQueue );
	
	return noErr;
}

static
QElemPtr find_an_event( short mask : __D0, EventRecord* event : __A0 )
{
	QElemPtr it = EventQueue.qHead;
	
	while ( it != NULL )
	{
		EvQEl* el = (EvQEl*) it;
		
		if ( (1 << el->evtQWhat) & mask )
		{
			EventRecord* ev = (EventRecord*) &el->evtQWhat;
			
			*event = *ev;
			
			return it;
		}
		
		it = it->qLink;
	}
	
	event->what      = nullEvent;
	event->message   = 0;
	event->when      = get_Ticks();
	event->where     = Mouse;
	event->modifiers = get_modifiers();
	
	return NULL;
}

long GetOSEvent_patch( short         trap  : __D1,
                       short         mask  : __D0,
                       EventRecord*  event : __A0 )
{
	QElemPtr it = find_an_event( mask, event );
	
	/*
		Our custom _WaitOSEvent and _WaitOSEventAvail are like the
		originals, except they wait politely for a matching event.
	*/
	
	if ( trap & kOSEventWaiting )
	{
		while ( it == NULL )
		{
			reactor_wait( NULL );
			
			it = find_an_event( mask, event );
		}
	}
	
	/*
		_OSEventAvail is A030 and _GetOSEvent is A031,
		so if the low bit is set, dequeue the event.
	*/
	
	if ( it != NULL  &&  (trap & 1) )
	{
		dequeue_event( it );
	}
	
	enable_SystemTask_throttle();
	
	return it ? 0 : -1;
}

void FlushEvents_patch( unsigned long masks : __D0 )
{
	QElemPtr it = EventQueue.qHead;
	
	while ( it != NULL )
	{
		QElemPtr next = it->qLink;
		
		EvQEl* el = (EvQEl*) it;
		
		if ( (1 << el->evtQWhat) & masks )
		{
			dequeue_event( it );
		}
		
		it = next;
	}
}
