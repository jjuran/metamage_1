/*
	Devices.cc
	----------
*/

#include "Devices.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif


short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return pb->ioResult = notOpenErr;
}

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}
