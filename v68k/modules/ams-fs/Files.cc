/*
	Files.cc
	--------
*/

#include "Files.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <string.h>


#define VOLNAME  "\p" "Macintosh HD"


enum
{
	kHFSFlagMask = 0x0200,
};

short GetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 )
{
	if ( trap_word & kHFSFlagMask )
	{
		pb->ioWDProcID  = 0;
		pb->ioWDVRefNum = -1;
		pb->ioWDDirID   = 2;
	}
	
	if ( pb->ioNamePtr )
	{
		memcpy( pb->ioNamePtr, VOLNAME, sizeof VOLNAME );
	}
	
	pb->ioVRefNum = -1;
	
	return pb->ioResult = noErr;
}

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	return pb->ioResult = extFSErr;
}

short Open_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	return pb->ioResult = extFSErr;
}
