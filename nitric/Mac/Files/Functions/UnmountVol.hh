/*
	Mac/Files/Functions/UnmountVol.hh
	---------------------------------
*/

#ifndef MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH
#define MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH

// Nitrogen
#include "Mac/Files/Types/FSVolumeRefNum.hh"


namespace Mac
{
	
	void UnmountVol( ConstStr63Param volName = NULL );
	
	void UnmountVol( FSVolumeRefNum vRefNum );
	
	//void UnmountVol( FSDriveIndex drive );
	
}

#endif

