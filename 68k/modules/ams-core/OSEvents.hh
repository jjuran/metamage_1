/*
	OSEvents.hh
	-----------
*/

#ifndef OSEVENTS_HH
#define OSEVENTS_HH

struct EventRecord;

short PostEvent_patch( short what : __A0, long msg : __D0 );

char OSEventAvail_patch( short m : __D0, EventRecord* ev : __A0 );
char GetOSEvent_patch  ( short m : __D0, EventRecord* ev : __A0 );

void FlushEvents_patch( unsigned long masks : __D0 );

#endif
