/*
	Devices.hh
	----------
*/

#ifndef DEVICES_HH
#define DEVICES_HH

struct CntrlParam;
struct IOParam;

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 );

short DRVR_IO_patch( short trap_word : __D1, IOParam* pb : __A0 );

short Read_patch( short trap_word : __D1, IOParam* pb : __A0 );
short Write_patch( short trap_word : __D1, IOParam* pb : __A0 );

short Status_patch( short trap_word : __D1, CntrlParam* pb : __A0 );

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 );

#endif
