/*
	OSEvents.hh
	-----------
*/

#ifndef OSEVENTS_HH
#define OSEVENTS_HH

struct EventRecord;

pascal short PostEvent_patch( short what : __A0, long msg : __D0 ) : __D0;

pascal char OSEventAvail_patch( short m : __D0, EventRecord* ev : __A0 ) : __D0;
pascal char GetOSEvent_patch  ( short m : __D0, EventRecord* ev : __A0 ) : __D0;

#endif
