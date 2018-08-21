/*
	Devices.hh
	----------
*/

#ifndef DEVICES_HH
#define DEVICES_HH

struct IOParam;

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 );

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 );

#endif
