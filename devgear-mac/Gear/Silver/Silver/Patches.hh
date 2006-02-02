/*
	
	Patches.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHES_HH
#define SILVER_PATCHES_HH

#include "Silver/Traps.hh"
#include "Silver/Patch.hh"


namespace Silver
{
	
	#define TrapName(name)  name ## Trap
	#define PatchName(name)  name ## Patch
	#define DefinePatch(n)  typedef Patch<TrapName(n)> PatchName(n)
	
	DefinePatch(DrawText);
	DefinePatch(GetNextEvent);
	DefinePatch(InsertMenu);
	DefinePatch(MenuKey);
	DefinePatch(MenuSelect);
	DefinePatch(StillDown);
	DefinePatch(TEActivate);
	DefinePatch(TEClick);
	DefinePatch(TEKey);
	DefinePatch(TEPaste);
	DefinePatch(AppendResMenu);
	DefinePatch(TESetSelect  );
	DefinePatch(SystemMenu  );
	
	#undef TrapName
	#undef PatchName
	#undef DefinePatch
}

#endif

