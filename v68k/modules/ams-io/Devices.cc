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


typedef OSErr (*IODoneProcPtr)( DCtlEntry* dce : __A1, OSErr err : __D0 );

IODoneProcPtr JIODone : 0x08FC;


static
OSErr IODone_handler( DCtlEntry* dce : __A1, OSErr err : __D0 )
{
	return err;
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
	const int noQueueMask   = 1 << noQueueBit;
	const int asyncTrapMask = 1 << asyncTrpBit;
	
	pb->ioTrap = trap_word;
	
	const short immed = trap_word & noQueueMask;
	const short async = trap_word & asyncTrapMask;
	
	if ( ! async )
	{
		pb->ioCompletion = NULL;
	}
	
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
	
	int8_t bit = command - 2;
	
	if ( ! (dce->dCtlFlags & (0x100 << bit)) )
	{
		/*
			The driver doesn't implement this call.
			
			        Read    Write   Control Status
			Trap:   A002    A003    A004    A005
			Bit#:      0       1       2       3
			OSErr:   -19     -20     -17     -18
		*/
		
		return (bit ^ 1) - 20;
	}
	
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
	JIODone = &IODone_handler;
	
	INSTALL_DRIVER( CIn  );
	INSTALL_DRIVER( COut );
}
