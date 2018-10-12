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

Open_ProcPtr old_Open;
IO_ProcPtr   old_Close;
IO_ProcPtr   old_Read;
IO_ProcPtr   old_Write;

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

short FlushVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	return pb->ioResult = extFSErr;
}

short Open_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	if ( trap_word & kHFSFlagMask )
	{
		// not a driver
	}
	else if ( pb->ioNamePtr  &&  *pb->ioNamePtr  &&  pb->ioNamePtr[ 1 ] == '.' )
	{
		// maybe a driver
		
		return old_Open( trap_word, pb );
	}
	
	return pb->ioResult = extFSErr;
}

short Read_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Read( trap_word, pb );
	}
	
	return pb->ioResult = rfNumErr;
}

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Write( trap_word, pb );
	}
	
	return pb->ioResult = rfNumErr;
}

short Close_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Close( trap_word, pb );
	}
	
	return pb->ioResult = rfNumErr;
}
