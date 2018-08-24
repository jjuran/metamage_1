/*
	Devices.cc
	----------
*/

#include "Devices.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// POSIX
#include <unistd.h>


#define LENGTH( array )  (sizeof array / sizeof *array)


typedef OSErr (*driver_routine)( short trap_word : __D1, IOParam* pb : __A0 );

#define CIn_write  NULL

static
OSErr COut_write( short trap_word : __D1, IOParam* pb : __A0 )
{
	ssize_t n_written = write( STDOUT_FILENO, pb->ioBuffer, pb->ioReqCount );
	
	if ( n_written < 0 )
	{
		return pb->ioResult = ioErr;
	}
	
	pb->ioActCount = n_written;
	
	return pb->ioResult = noErr;
}

struct driver
{
	const uint8_t*  name;
	driver_routine  write;
};

#define DRIVER( name )  { "\p." #name, name##_write }

static const driver drivers[] =
{
	DRIVER( CIn  ),
	DRIVER( COut ),
};

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.write == NULL )
	{
		return pb->ioResult = writErr;
	}
	
	return d.write( trap_word, pb );
}

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}
