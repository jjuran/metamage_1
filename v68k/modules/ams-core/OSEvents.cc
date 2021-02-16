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


Ptr  SysEvtBuf  : 0x0146;
QHdr EventQueue : 0x014A;
short SysEvtCnt : 0x0154;

UInt32 Ticks   : 0x016A;
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
	event->when      = Ticks;
	event->where     = Mouse;
	event->modifiers = get_modifiers();
	
	return NULL;
}

char OSEventAvail_patch( short mask : __D0, EventRecord* event : __A0 )
{
	QElemPtr it = find_an_event( mask, event );
	
	return -(it == NULL);
}

char GetOSEvent_patch( short mask : __D0, EventRecord* event : __A0 )
{
	QElemPtr it = find_an_event( mask, event );
	
	if ( it != NULL )
	{
		dequeue_event( it );
	}
	
	EventQueue.qFlags |= 1;  // GetOSEvent was here
	
	return -(it == NULL);
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
