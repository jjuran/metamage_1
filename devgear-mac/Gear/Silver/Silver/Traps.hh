/*
	
	Traps.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_TRAPS_HH
#define SILVER_TRAPS_HH

// Universal Interfaces
#include <Traps.h>

// Silver
#include "Silver/Procs.hh"


namespace Silver
{
	
	template < short trapNum, class ProcType >
	struct Trap
	{
		typedef ProcType ProcPtr;
		
		static short TrapWord()  { return trapNum; }
	};
	
	#define DEFINE_TRAP( name )  typedef Trap< _ ## name, name ## ProcPtr > name ## Trap
	
	DEFINE_TRAP( DrawText      );
	DEFINE_TRAP( GetNextEvent  );
	DEFINE_TRAP( InsertMenu    );
	DEFINE_TRAP( MenuKey       );
	DEFINE_TRAP( MenuSelect    );
	DEFINE_TRAP( StillDown     );
	DEFINE_TRAP( TEActivate    );
	DEFINE_TRAP( TEClick       );
	DEFINE_TRAP( TEKey         );
	DEFINE_TRAP( TEPaste       );
	DEFINE_TRAP( AppendResMenu );
	DEFINE_TRAP( TESetSelect   );
	DEFINE_TRAP( SystemMenu    );
	DEFINE_TRAP( ExitToShell   );
	
	#undef DEFINE_TRAP
	
}

#endif

