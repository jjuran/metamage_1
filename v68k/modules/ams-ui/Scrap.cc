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


#pragma mark -
#pragma mark Keeping the Desk Scrap on the Disk
#pragma mark -

pascal long UnloadScrap_patch()
{
	return noErr;
}

pascal long LoadScrap_patch()
{
	return noErr;
}

#pragma mark -
#pragma mark Writing to the Desk Scrap
#pragma mark -

pascal long ZeroScrap_patch()
{
	return noErr;
}

pascal long PutScrap_patch( long length, ResType type, const char* src )
{
	return noErr;
}

#pragma mark -
#pragma mark Reading from the Desk Scrap
#pragma mark -

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
