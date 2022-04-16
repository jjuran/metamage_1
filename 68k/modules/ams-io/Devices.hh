/*
	Devices.hh
	----------
*/

#ifndef DEVICES_HH
#define DEVICES_HH

struct IOParam;

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 );
short Close_patch( short trap_word : __D1, IOParam* pb : __A0 );

short DRVR_IO_patch( short trap_word : __D1, IOParam* pb : __A0 );

#define Read_patch     DRVR_IO_patch
#define Write_patch    DRVR_IO_patch
#define Control_patch  DRVR_IO_patch
#define Status_patch   DRVR_IO_patch
#define KillIO_patch   DRVR_IO_patch

void install_drivers();

#endif
