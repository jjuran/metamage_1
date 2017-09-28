/*
	Scrap.cc
	--------
*/

#include "Scrap.hh"

// Mac OS
#ifndef __SCRAP__
#include <Scrap.h>
#endif


pascal long ZeroScrap_patch()
{
	return noErr;
}
