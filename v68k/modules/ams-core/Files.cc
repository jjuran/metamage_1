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


pascal short GetVol_patch( VolumeParam* pb : __A0 ) : __D0
{
	if ( pb->ioNamePtr )
	{
		memcpy( pb->ioNamePtr, VOLNAME, sizeof VOLNAME );
	}
	
	pb->ioVRefNum = -1;
	
	return pb->ioResult = noErr;
}

pascal short Create_patch( FileParam* pb : __A0 ) : __D0
{
	return pb->ioResult = extFSErr;
}

pascal short Open_patch( FileParam* pb : __A0 ) : __D0
{
	return pb->ioResult = extFSErr;
}
