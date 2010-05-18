/*
	Mac/Files/Functions/FlushVol.hh
	-------------------------------
*/

#ifndef MAC_FILES_FUNCTIONS_FLUSHVOL_HH
#define MAC_FILES_FUNCTIONS_FLUSHVOL_HH

// Nitrogen
#include "Mac/Files/Types/FSVolumeRefNum.hh"


namespace Mac
{
	
	void FlushVol( ConstStr63Param volName = NULL );
	
	void FlushVol( FSVolumeRefNum vRefNum );
	
	//void FlushVol( FSDriveIndex drive );
	
}

#endif

