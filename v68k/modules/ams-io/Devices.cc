/*
	Devices.cc
	----------
*/

#include "Devices.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// Standard C
#include <string.h>

// ams-io
#include "Console.hh"


#define LENGTH( array )  (sizeof array / sizeof *array)


typedef OSErr (*driver_routine)( short trap_word : __D1, IOParam* pb : __A0 );

typedef OSErr (*cntrl_routine)( short trap_word : __D1, CntrlParam* pb : __A0 );

struct driver
{
	const uint8_t*  name;
	driver_routine  prime;
	cntrl_routine   status;
};

#define DRIVER( d )  { "\p." #d, d##_prime, d##_status }

static const driver drivers[] =
{
	DRIVER( CIn  ),
	DRIVER( COut ),
};

short Read_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.prime == NULL )
	{
		return pb->ioResult = readErr;
	}
	
	return d.prime( trap_word, pb );
}

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.prime == NULL )
	{
		return pb->ioResult = writErr;
	}
	
	return d.prime( trap_word, pb );
}

short Status_patch( short trap_word : __D1, CntrlParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioCRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.status == NULL )
	{
		return pb->ioResult = statusErr;
	}
	
	return d.status( trap_word, pb );
}

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioNamePtr == NULL )
	{
		return pb->ioResult = paramErr;
	}
	
	for ( short i = 0;  i < LENGTH( drivers );  ++i )
	{
		const driver& d = drivers[ i ];
		
		if ( memcmp( pb->ioNamePtr, d.name, 1 + *d.name ) == 0 )
		{
			pb->ioRefNum = -100 - i;
			
			return pb->ioResult = noErr;
		}
	}
	
	return pb->ioResult = fnfErr;
}
