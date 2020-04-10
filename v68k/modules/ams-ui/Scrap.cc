/*
	Scrap.cc
	--------
*/

#include "Scrap.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


short  TEScrpLength : 0x0AB0;
Handle TEScrpHandle : 0x0AB4;


pascal long ZeroScrap_patch()
{
	return noErr;
}

pascal long PutScrap_patch( long length, ResType type, const char* src )
{
	return noErr;
}

pascal long GetScrap_patch( char** dst, ResType type, long* offset )
{
	*offset = 0;
	
	if ( dst != NULL )
	{
		if ( OSErr err = PtrToXHand( *TEScrpHandle, dst, TEScrpLength ) )
		{
			return err;
		}
	}
	
	return TEScrpLength;
}
