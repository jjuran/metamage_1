/*
	SysError.hh
	-----------
*/

#ifndef SYSERROR_HH
#define SYSERROR_HH

void SysError_patch( short error : __D0 );

int set_syserr_handler();

#endif
