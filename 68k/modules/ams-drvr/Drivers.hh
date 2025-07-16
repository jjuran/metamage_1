/*
	Drivers.hh
	----------
*/

#ifndef DRIVERS_HH
#define DRIVERS_HH

struct FileParam;
struct IOParam;

typedef short (*Open_ProcPtr)( short trap_word : __D1, FileParam* pb : __A0 );

extern Open_ProcPtr old_Open;

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 );

#endif
