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
	
	template < UInt16 > struct TrapTraits;
	
	template <> struct TrapTraits< _GetNextEvent >
	{
		typedef pascal short (*ProcPtr)( EventMask, EventRecord* );
		
		typedef short (*PatchProcPtr)( EventMask, EventRecord*, ProcPtr );
	};
	
	template <> struct TrapTraits< _TEActivate >
	{
		typedef pascal void (*ProcPtr)( TEHandle );
		
		typedef void (*PatchProcPtr)( TEHandle, ProcPtr );
	};
	
	template <> struct TrapTraits< _TEClick >
	{
		typedef pascal void (*ProcPtr)( Point, short, TEHandle );
		
		typedef void (*PatchProcPtr)( Point, short, TEHandle, ProcPtr );
	};
	
	template <> struct TrapTraits< _TEKey >
	{
		typedef pascal void (*ProcPtr)( short, TEHandle );
		
		typedef void (*PatchProcPtr)( short, TEHandle, ProcPtr );
	};
	
	
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

