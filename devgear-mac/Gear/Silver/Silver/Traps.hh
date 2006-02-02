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
	
	#define _Name(       name )  _ ## name
	#define ProcPtrName( name )  name ## ProcPtr
	#define TrapName(    name )  name ## Trap
	#define DefineTrap(  n    )  typedef Trap< _Name( n ), ProcPtrName( n ) > TrapName( n )
	
	DefineTrap( DrawText      );
	DefineTrap( GetNextEvent  );
	DefineTrap( InsertMenu    );
	DefineTrap( MenuKey       );
	DefineTrap( MenuSelect    );
	DefineTrap( StillDown     );
	DefineTrap( TEActivate    );
	DefineTrap( TEClick       );
	DefineTrap( TEKey         );
	DefineTrap( TEPaste       );
	DefineTrap( AppendResMenu );
	DefineTrap( TESetSelect   );
	DefineTrap( SystemMenu   );
	
	#undef _Name
	#undef ProcPtrName
	#undef TrapName
	#undef DefineTrap
	
}

#endif

