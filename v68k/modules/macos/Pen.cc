/*
	Pen.cc
	------
*/

#include "Pen.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// macos
#include "QDGlobals.hh"


pascal void GetPenState_patch( PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*state = *(const PenState*) &thePort->pnLoc;
}

pascal void SetPenState_patch( const PenState* state )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	*(PenState*) &thePort->pnLoc = *state;
}
