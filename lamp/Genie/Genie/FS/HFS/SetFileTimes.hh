/*
	Genie/FS/HFS/SetFileTimes.hh
	----------------------------
*/

#ifndef GENIE_FS_HFS_SETFILETIMES_HH
#define GENIE_FS_HFS_SETFILETIMES_HH

// Standard C
#include <time.h>

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif


namespace Genie
{
	
	void SetFileTimes( Mac::FSVolumeRefNum   vRefNum,
	                   Mac::FSDirID          dirID,
	                   const unsigned char*  name );
	
	void SetFileTimes( Mac::FSVolumeRefNum    vRefNum,
	                   Mac::FSDirID           dirID,
	                   const unsigned char*   name,
	                   const struct timespec  times[2] );
	
}

#endif

