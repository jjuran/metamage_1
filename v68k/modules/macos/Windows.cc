/*
	Windows.cc
	----------
*/

#include "Windows.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


GrafPtr WMgrPort : 0x09DE;


pascal void InitWindows_patch()
{
	WMgrPort = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	OpenPort( WMgrPort );
}
