/*
	Events.hh
	---------
*/

#ifndef EVENTS_HH
#define EVENTS_HH

struct EventRecord;
struct MacRegion;
struct Point;

extern unsigned long GetNextEvent_throttle;  // minimum ticks between calls

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event );

pascal unsigned char EventAvail_patch( unsigned short  eventMask,
                                       EventRecord*    event );

pascal unsigned char WaitNextEvent_patch( unsigned short  eventMask,
                                          EventRecord*    event,
                                          unsigned long   sleep,
                                          MacRegion**     mouseRgn );

pascal void GetMouse_patch( Point* loc );

pascal char Button_patch();
pascal char StillDown_patch();
pascal char WaitMouseUp_patch();

pascal void GetKeys_patch( unsigned long* keys );

pascal long TickCount_patch();

#endif
