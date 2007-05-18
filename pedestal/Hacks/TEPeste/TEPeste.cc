/*
	
	TEPeste
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <Resources.h>
#include <Sound.h>
#include <TextEdit.h>

// Standard C/C++
#include <cstring>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


namespace Ag = Silver;

using namespace Ag::Trap_ProcPtrs;


static bool gLastTimeWasAGag;


static bool ShouldDoAGag()
{
	return !gLastTimeWasAGag  &&  (TickCount() & 3) == 0;
}

static const char* PickAGag( short which )
{
	switch ( which )
	{
		case 0:
			return "Boo!";
		
		case 1:
			return "Whazzuuup!";
		
		case 2:
			return "Nobody expects the Spanish Inquisition!";
		
		case 3:
			return "You REALLY ought to get some sleep...";
		
		case 4:
			return "¥ Vote for Joshua Juran ¥";
		
		default:
			return "";
	}
}

static void Payload( TEHandle hTE )
{
	const char* gag = PickAGag( TickCount() % 5 );
	
	long len = std::strlen( gag );
	long start = hTE[0]->selStart;
	
	SysBeep( 30 );
	
	TEDelete( hTE );
	
	TEInsert( gag, len, hTE );
	
	TESetSelect( start, start + len, hTE );
	
	UInt32 now = TickCount();
	
	while ( TickCount() < now + 30 )
	{
		//
	}
}

namespace
{
	
	void PatchedTEPaste( TEHandle hTE, TEPasteProcPtr nextHandler )
	{
		if ( (gLastTimeWasAGag = ShouldDoAGag()) )
		{
			Payload( hTE );
		}
		
		nextHandler( hTE );
	}
	
}

static OSErr Installer()
{
	Ag::TrapPatch< _TEPaste, PatchedTEPaste >::Install();
	
	return noErr;
}

void main()
{
	(void) Ag::Install( Installer );
}

