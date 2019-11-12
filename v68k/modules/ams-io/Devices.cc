/*
	Devices.cc
	----------
*/

#include "Devices.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-io
#include "Console.hh"
#include "DRVR.hh"
#include "options.hh"
#include "Sound.hh"
#include "UnitTable.hh"


namespace logofwar
{

static inline
void print( const unsigned char* s )
{
	print( (const char*) s + 1, *s );
}

}

using logofwar::print;

typedef OSErr (*IODoneProcPtr)( DCtlEntry* dce : __A1, OSErr err : __D0 );

UInt16 SdVolEnb : 0x0260;

Byte SdVolume : 0x0260;
Byte SdEnable : 0x0261;

IODoneProcPtr JIODone : 0x08FC;


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


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

static inline
asm void call_completion_routine( void* pb : __A0,
                                  short err : __D0,
                                  void* proc : __A1 )
{
	JSR      (A1)
}

static
OSErr IOComplete( IOParam* pb : __A0, OSErr err : __D0 )
{
	pb->ioResult = err;
	
	if ( pb->ioCompletion )
	{
		call_completion_routine( pb, err, pb->ioCompletion );
	}
	
	return err;
}

static
OSErr IODone_handler( DCtlEntry* dce : __A1, OSErr err : __D0 )
{
	QElemPtr head = dce->dCtlQHdr.qHead;
	
	if ( OSErr err = Dequeue( head, &dce->dCtlQHdr ) )
	{
		/*
			Another IODone() call beat us to the punch.  Bail out now, so we
			don't call the completion handler a second time.
		*/
		
		return err;
	}
	
	IOParam* pb = (IOParam*) head;
	
	return IOComplete( pb, err );
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
		
		NOTICE = "Open \"", pb->ioNamePtr, "\" -> ", pb->ioRefNum;
		
		trap_word |= 1 << noQueueBit;  // treat Open like an immediate call
		
		return pb->ioResult = DRVR_IO_patch( trap_word, pb );
	}
	
	WARNING = "Open \"", pb->ioNamePtr, "\": driver not found";
	
	return pb->ioResult = fnfErr;
}

short DRVR_IO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	const int noQueueMask   = 1 << noQueueBit;
	const int asyncTrapMask = 1 << asyncTrpBit;
	
	pb->ioTrap = trap_word;
	
	uint8_t command = trap_word;
	
	const bool killIO = command == kKillIOCommand;
	
	const short immed = trap_word & noQueueMask  ||  killIO;
	const short async = trap_word & asyncTrapMask;
	
	if ( ! async )
	{
		pb->ioCompletion = NULL;
	}
	
	DCtlHandle h = GetDCtlEntry( pb->ioRefNum );
	
	if ( h == NULL )
	{
		return IOComplete( pb, unitEmptyErr );
	}
	
	DCtlEntry* dce = *h;
	
	DRVRHeader* drvr = get_driver( dce );
	
	if ( drvr == NULL )
	{
		return IOComplete( pb, badUnitErr );
	}
	
	if ( killIO )
	{
		command = kControlCommand;
		
		CntrlParam* cntrl = (CntrlParam*) pb;
		
		cntrl->csCode = killCode;
	}
	
	int8_t bit = command - 2;
	
	if ( bit >= 0  &&  ! (dce->dCtlFlags & (0x100 << bit)) )
	{
		/*
			The driver doesn't implement this call.
			
			        Read    Write   Control Status
			Trap:   A002    A003    A004    A005
			Bit#:      0       1       2       3
			OSErr:   -19     -20     -17     -18
		*/
		
		return IOComplete( pb, (bit ^ 1) - 20 );
	}
	
	if ( ! immed )
	{
		pb->qType = ioQType;
		
		Enqueue( (QElemPtr) pb, &dce->dCtlQHdr );
	}
	
	short offset = 0;
	
	switch ( command )
	{
		case kOpenCommand:
			offset = drvr->drvrOpen;
			break;
		
		case kCloseCommand:
			offset = drvr->drvrClose;
			break;
		
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
	
	OSErr err = call_DRVR( trap_word, entry_point, pb, dce );
	
	if ( killIO )
	{
		while ( QElemPtr head = dce->dCtlQHdr.qHead )
		{
			IODone( dce, abortErr );
		}
	}
	
	if ( immed )
	{
		return pb->ioResult = err;
	}
	
	if ( ! async )
	{
		while ( pb->ioResult > 0 )
		{
			UInt32 dummy;
			Delay( -1, &dummy );  // calls reactor_wait() once
		}
		
		err = pb->ioResult;
	}
	
	return err;
}

#define INSTALL_SYS_DRIVER( d, i )  \
	install( make_DRVR( "\p." #d,  \
	d##_open, d##_prime, d##_control, d##_status, d##_close  \
	), i )

#define INSTALL_DRIVER( d )  \
	install( make_DRVR( "\p." #d,  \
	d##_open, d##_prime, d##_control, d##_status, d##_close  \
	) )

void install_drivers()
{
	JIODone = &IODone_handler;
	
	if ( sound_fd >= 0 )
	{
		INSTALL_SYS_DRIVER( Sound, 3 );
		
		short refnum;
		OpenDriver( "\p.Sound", &refnum );
		
		TBTRAP( SysBeep );  // A9C8
		
		SdVolEnb = -1;
	}
	
	INSTALL_DRIVER( CIn  );
	INSTALL_DRIVER( COut );
}
