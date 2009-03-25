/*
	
	PatchUtils.cc
	
	Joshua Juran
	
*/

#include "Silver/PatchUtils.hh"

// Mac OS Universal Interfaces
#include <Patches.h>


#pragma exceptions off


namespace Silver
{
	
	ProcPtr ApplyTrapPatch_( short trap, ProcPtr patchPtr )
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
	
}

