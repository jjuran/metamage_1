/*
	
	TEPeste
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// <Sound.h>
extern "C" pascal void SysBeep( short ticks )  ONEWORDINLINE( 0xA9C8 );

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/delay.hh"
#include "mac_sys/trap_address.hh"


#pragma exceptions off


static UniversalProcPtr old_TEPaste;

static const char* gGags[] =
{
	"Boo!",
	"Whazzuuup!",
	"Nobody expects the Spanish Inquisition!",
	"You REALLY ought to get some sleep...",
	"\xA5 Vote for Josh Juran \xA5",  // bullets
	""
};

static const unsigned kCountOfGags = sizeof gGags / sizeof gGags[0] - 1;

static bool gLastTimeWasAGag;


static inline bool ShouldDoAGag()
{
	return !gLastTimeWasAGag  &&  (TickCount() & 3) == 0;
}

static inline const char* PickAGag( short which )
{
	return gGags[ which ];
}

static void Payload( TEHandle hTE )
{
	const char* gag = PickAGag( TickCount() % kCountOfGags );
	
	long len = strlen( gag );
	long start = hTE[0]->selStart;
	
	SysBeep( 30 );
	
	TEDelete( hTE );
	
	TEInsert( gag, len, hTE );
	
	TESetSelect( start, start + len, hTE );
	
	mac::sys::delay( 30 );
}

static
void TEPaste_handler( TEHandle hTE )
{
	if ( (gLastTimeWasAGag = ShouldDoAGag()) )
	{
		Payload( hTE );
	}
}

static
pascal
asm
void TEPaste_patch( TEHandle hTE )
{
	LINK     A6,#0
	
	MOVE.L   8(A6),-(SP)
	JSR      TEPaste_handler
	
	UNLK     A6
	
	MOVEA.L  old_TEPaste,A0
	JMP      (A0)
}

int main()
{
	Handle self = GetResource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_TEPaste = mac::sys::get_trap_address( _TEPaste );
	
	mac::sys::set_trap_address( (ProcPtr) TEPaste_patch, _TEPaste );
	
	return 0;
}
