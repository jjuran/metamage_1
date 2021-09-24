/*
	
	TEPeste
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// Standard C/C++
#include <cstring>

// mac-sys-utils
#include "mac_sys/delay.hh"

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace Ag = Silver;

using namespace Ag::Trap_ProcPtrs;


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
	
	long len = std::strlen( gag );
	long start = hTE[0]->selStart;
	
	SysBeep( 30 );
	
	TEDelete( hTE );
	
	TEInsert( gag, len, hTE );
	
	TESetSelect( start, start + len, hTE );
	
	mac::sys::delay( 30 );
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

int main()
{
	return Ag::Install( Installer );
}
