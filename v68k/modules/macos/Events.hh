/*
	Events.hh
	---------
*/

#ifndef EVENTS_HH
#define EVENTS_HH

struct EventRecord;

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event );

#endif
