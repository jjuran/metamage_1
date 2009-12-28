/*
	
	Traps.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_TRAPS_HH
#define SILVER_TRAPS_HH

#ifdef __MACOS__

// Universal Interfaces
#include <Traps.h>

// Silver
#include "Silver/Procs.hh"


namespace Silver
{
	
	template < class ProcPtr > struct PatchProc_Traits;
	
	template < class R >
	struct PatchProc_Traits< R (*)() >
	{
		static const int arity = 0;
		
		typedef R result_type;
		
		typedef pascal R (*ProcPtr)();
		
		typedef R (*PatchProcPtr)( ProcPtr );
	};
	
	template < class R, class P0 >
	struct PatchProc_Traits< R (*)( P0 ) >
	{
		static const int arity = 1;
		
		typedef R   result_type;
		typedef P0  param0_type;
		
		typedef pascal R (*ProcPtr)( P0 );
		
		typedef R (*PatchProcPtr)( P0, ProcPtr );
	};
	
	template < class R, class P0, class P1 >
	struct PatchProc_Traits< R (*)( P0, P1 ) >
	{
		static const int arity = 2;
		
		typedef R   result_type;
		typedef P0  param0_type;
		typedef P1  param1_type;
		
		typedef pascal R (*ProcPtr)( P0, P1 );
		
		typedef R (*PatchProcPtr)( P0, P1, ProcPtr );
	};
	
	template < class R, class P0, class P1, class P2 >
	struct PatchProc_Traits< R (*)( P0, P1, P2 ) >
	{
		static const int arity = 3;
		
		typedef R   result_type;
		typedef P0  param0_type;
		typedef P1  param1_type;
		typedef P2  param2_type;
		
		typedef pascal R (*ProcPtr)( P0, P1, P2 );
		
		typedef R (*PatchProcPtr)( P0, P1, P2, ProcPtr );
	};
	
	
	template < UInt16 > struct TrapTraits;
	
	#define DEFINE_TRAP_TRAITS( name )  template <> struct TrapTraits< _ ## name > : public PatchProc_Traits< name ## ProcPtr > {}
	
	DEFINE_TRAP_TRAITS( AppendResMenu );
	DEFINE_TRAP_TRAITS( DrawText      );
	DEFINE_TRAP_TRAITS( ExitToShell   );
	DEFINE_TRAP_TRAITS( GetNextEvent  );
	DEFINE_TRAP_TRAITS( InsertMenu    );
	DEFINE_TRAP_TRAITS( MenuKey       );
	DEFINE_TRAP_TRAITS( MenuSelect    );
	DEFINE_TRAP_TRAITS( StillDown     );
	DEFINE_TRAP_TRAITS( SystemMenu    );
	DEFINE_TRAP_TRAITS( TEActivate    );
	DEFINE_TRAP_TRAITS( TEClick       );
	DEFINE_TRAP_TRAITS( TECopy        );
	DEFINE_TRAP_TRAITS( TECut         );
	DEFINE_TRAP_TRAITS( TEInit        );
	DEFINE_TRAP_TRAITS( TEKey         );
	DEFINE_TRAP_TRAITS( TEPaste       );
	DEFINE_TRAP_TRAITS( TESetSelect   );
	
	#undef DEFINE_TRAP_TRAITS
	
}

#endif

#endif

