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
#include "UnitTable.hh"


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

static
asm
short call_DRVR( short  trap : __D1,
                 long   proc : __D2,
                 void*  pb   : __A0,
                 void*  dce  : __A1 )
{
	LINK     A6,#0
	
	MOVEM.L  D3-D7/A2-A4,-(SP)
	
	MOVEA.L  D2,A2
	JSR      (A2)
	
	MOVEM.L  (SP)+,D3-D7/A2-A4
	
	UNLK     A6
	RTS
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

short DRVR_IO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	DCtlHandle h = GetDCtlEntry( pb->ioRefNum );
	
	if ( h == NULL )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	DCtlEntry* dce = *h;
	
	DRVRHeader* drvr = get_driver( dce );
	
	if ( drvr == NULL )
	{
		return pb->ioResult = badUnitErr;
	}
	
	uint8_t command = trap_word;
	
	short offset = 0;
	
	switch ( command )
	{
		case kReadCommand:
		case kWriteCommand:
			offset = drvr->drvrPrime;
			break;
		
		case kControlCommand:
			offset = drvr->drvrCtl;
			break;
		
		case kStatusCommand:
			offset = drvr->drvrStatus;
			break;
		
		default:
			break;
	}
	
	long entry_point = (long) drvr + offset;
	
	return call_DRVR( trap_word, entry_point, pb, dce );
}
