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
	
	#define DEFINE_PATCH( name )  typedef Patch< name ## Trap > name ## Patch
	
	DEFINE_PATCH( DrawText      );
	DEFINE_PATCH( GetNextEvent  );
	DEFINE_PATCH( InsertMenu    );
	DEFINE_PATCH( MenuKey       );
	DEFINE_PATCH( MenuSelect    );
	DEFINE_PATCH( StillDown     );
	DEFINE_PATCH( TEActivate    );
	DEFINE_PATCH( TEClick       );
	DEFINE_PATCH( TEKey         );
	DEFINE_PATCH( TEPaste       );
	DEFINE_PATCH( AppendResMenu );
	DEFINE_PATCH( TESetSelect   );
	DEFINE_PATCH( SystemMenu    );
	DEFINE_PATCH( ExitToShell   );
	
	#undef DEFINE_PATCH
	#undef TrapName
	#undef PatchName
	#undef DefinePatch
}

#endif

