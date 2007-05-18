/*
	
	PatchUtils.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHUTILS_HH
#define SILVER_PATCHUTILS_HH

// Universal Interfaces
#include <Traps.h>


namespace Silver
{
	
	static ProcPtr ApplyTrapPatch_( short trap, ProcPtr patchPtr )
	{
		if ( patchPtr == NULL )
		{
			return NULL;
		}
		
		TrapType trapType = (trap & 0x0800) ? ToolTrap : OSTrap;
		
		ProcPtr trapPtr = ::NGetTrapAddress( trap, trapType );
		::NSetTrapAddress( patchPtr, trap, trapType );
		
		return trapPtr;
	}
	
	inline void RemoveTrapPatch_( short trap, ProcPtr patchPtr )
	{
		ApplyTrapPatch_( trap, patchPtr );
	}
	
	template < class PatchedProcPtr >
	PatchedProcPtr ApplyTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		ProcPtr trapPtr = ApplyTrapPatch_( trap,
		                                   reinterpret_cast< ProcPtr >( patchPtr ) );
		
		return reinterpret_cast< PatchedProcPtr >( trapPtr );
	}
	
	template < class PatchedProcPtr >
	void RemoveTrapPatch( short trap, PatchedProcPtr patchPtr )
	{
		RemoveTrapPatch_( trap, reinterpret_cast< ProcPtr >( patchPtr ) );
	}
	
}
#endif

