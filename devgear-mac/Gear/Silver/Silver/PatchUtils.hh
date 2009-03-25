/*
	
	PatchUtils.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHUTILS_HH
#define SILVER_PATCHUTILS_HH

// Universal Interfaces
#include <MacTypes.h>


namespace Silver
{
	
	ProcPtr ApplyTrapPatch_( short trap, ProcPtr patchPtr );
	
	inline void RemoveTrapPatch_( short trap, ProcPtr patchPtr )
	{
		ApplyTrapPatch_( trap, patchPtr );
	}
	
	template < class PatchedProcPtr >
	inline PatchedProcPtr ApplyTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		ProcPtr trapPtr = ApplyTrapPatch_( trap,
		                                   reinterpret_cast< ProcPtr >( patchPtr ) );
		
		return reinterpret_cast< PatchedProcPtr >( trapPtr );
	}
	
	template < class PatchedProcPtr >
	inline void RemoveTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		RemoveTrapPatch_( trap, reinterpret_cast< ProcPtr >( patchPtr ) );
	}
	
}

#endif

