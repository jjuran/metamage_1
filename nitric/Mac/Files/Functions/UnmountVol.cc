/*
	Mac/Files/Functions/UnmountVol.cc
	---------------------------------
*/

#include "Mac/Files/Functions/UnmountVol.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Mac
{
	
	void UnmountVol( ConstStr63Param volName )
	{
		ThrowOSStatus( ::UnmountVol( volName, 0 ) );
	}
	
	void UnmountVol( FSVolumeRefNum vRefNum )
	{
		ThrowOSStatus( ::UnmountVol( NULL, vRefNum ) );
	}
	
}

