/*
	Events.hh
	---------
*/

#ifndef EVENTS_HH
#define EVENTS_HH

struct EventRecord;
struct MacRegion;
struct Point;

typedef unsigned char Boolean;

typedef MacRegion** RgnHandle;


extern unsigned long GetNextEvent_throttle;  // minimum ticks between calls

pascal Boolean GetNextEvent_patch( unsigned short  eventMask,
                                   EventRecord*    event );

pascal Boolean EventAvail_patch( unsigned short  eventMask,
                                 EventRecord*    event );

pascal Boolean WaitNextEvent_patch( unsigned short  eventMask,
                                    EventRecord*    event,
                                    unsigned long   sleep,
                                    RgnHandle       mouseRgn );

pascal void GetMouse_patch( Point* loc );

pascal Boolean Button_patch();
pascal Boolean StillDown_patch();
pascal Boolean WaitMouseUp_patch();

pascal void GetKeys_patch( unsigned long* keys );

pascal long TickCount_patch();

#endif
