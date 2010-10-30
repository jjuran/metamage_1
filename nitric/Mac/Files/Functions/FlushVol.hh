/*
	Mac/Files/Functions/FlushVol.hh
	-------------------------------
*/

#ifndef MAC_FILES_FUNCTIONS_FLUSHVOL_HH
#define MAC_FILES_FUNCTIONS_FLUSHVOL_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif


namespace Mac
{
	
	void FlushVol( ConstStr63Param volName = NULL );
	
	void FlushVol( FSVolumeRefNum vRefNum );
	
	//void FlushVol( FSDriveIndex drive );
	
}

#endif

