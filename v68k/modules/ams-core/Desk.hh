/*
	Desk.hh
	-------
*/

#ifndef DESK_HH
#define DESK_HH

typedef unsigned char Boolean;

struct EventRecord;

pascal Boolean SystemEdit_patch( short cmd );

pascal void SystemTask_patch();

pascal Boolean SystemEvent_patch( const EventRecord* event );

#endif
