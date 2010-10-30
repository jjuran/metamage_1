/*
	Mac/Files/Functions/UnmountVol.hh
	---------------------------------
*/

#ifndef MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH
#define MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif


namespace Mac
{
	
	void UnmountVol( ConstStr63Param volName = NULL );
	
	void UnmountVol( FSVolumeRefNum vRefNum );
	
	//void UnmountVol( FSDriveIndex drive );
	
}

#endif

