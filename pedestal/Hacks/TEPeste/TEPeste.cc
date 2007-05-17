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
#include "Silver/Patches.hh"


using Silver::PatchApplied;
using Silver::TEPastePatch;

static PatchApplied<TEPastePatch> gTEPastePatch;


static bool gLastTimeWasAGag;


static bool ShouldDoAGag()
{
	return !gLastTimeWasAGag  &&  (TickCount() & 3) == 0;
}

static const char* PickAGag(short which)
{
	switch (which) {
	case 0:
		return "Boo!";
		break;
	case 1:
		return "Whazzuuup!";
		break;
	case 2:
		return "Nobody expects the Spanish Inquisition!";
		break;
	case 3:
		return "You REALLY ought to get some sleep...";
		break;
	case 4:
		return "¥ Vote for Joshua Juran ¥";
		break;
	default:
		return "";
	}
}

static void Payload(TEHandle hTE)
{
	const char* gag = PickAGag(TickCount() % 5);
	long len = std::strlen(gag);
	long start = (**hTE).selStart;
	
	SysBeep( 30 );
	TEDelete(hTE);
	TEInsert(gag, len, hTE);
	TESetSelect(start, start + len, hTE);
	
	start = TickCount();
	while (TickCount() < start + 30) {
		
	}
}

static pascal void PatchedTEPaste(TEHandle hTE)
{
	MyA4 a4;
	
	if ( (gLastTimeWasAGag = ShouldDoAGag()) ) {
		Payload(hTE);
	}
	gTEPastePatch.Original()(hTE);
}

static bool Install()
{
	bool locked = LoadAndLock();
	if (!locked)
		return false;
	
	MyA4 a4;
	
	gTEPastePatch = TEPastePatch(PatchedTEPaste);
	
	return true;
}

void main()
{
	bool installed = Install();
}

