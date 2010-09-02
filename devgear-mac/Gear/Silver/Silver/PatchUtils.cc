/*
	
	PatchUtils.cc
	
	Joshua Juran
	
*/

#include "Silver/PatchUtils.hh"

// Mac OS
#ifndef __PATCHES__
#include <Patches.h>
#endif

// iota
#ifdef __GNUC__
#include "iota/dummy.hh"
#endif


#pragma exceptions off


namespace Silver
{
	
#if !TARGET_API_MAC_CARBON
	
	UniversalProcPtr ApplyTrapPatch_( short trap, UniversalProcPtr patchPtr )
	{
		if ( patchPtr == NULL )
		{
			return NULL;
		}
		
		TrapType trapType = (trap & 0x0800) ? ToolTrap : OSTrap;
		
		UniversalProcPtr trapPtr = ::NGetTrapAddress( trap, trapType );
		
		::NSetTrapAddress( patchPtr, trap, trapType );
		
		return trapPtr;
	}
	
#endif
	
}

