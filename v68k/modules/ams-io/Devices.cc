/*
	Devices.cc
	----------
*/

#include "Devices.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// ams-io
#include "Console.hh"
#include "DRVR.hh"
#include "UnitTable.hh"


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
	
	short i = find_unit_entry_by_name( pb->ioNamePtr );
	
	if ( i >= 0 )
	{
		pb->ioRefNum = ~i;
		
		return pb->ioResult = noErr;
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

#define INSTALL_DRIVER( d )  \
	install( make_DRVR( "\p." #d, 0, d##_prime, 0, d##_status, 0 ) )

void install_drivers()
{
	INSTALL_DRIVER( CIn  );
	INSTALL_DRIVER( COut );
}
