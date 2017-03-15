/*
	Events.hh
	---------
*/

#ifndef EVENTS_HH
#define EVENTS_HH

struct EventRecord;
struct MacRegion;

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event );

pascal unsigned char WaitNextEvent_patch( unsigned short  eventMask,
                                          EventRecord*    event,
                                          unsigned long   sleep,
                                          MacRegion**     mouseRgn );

#endif
