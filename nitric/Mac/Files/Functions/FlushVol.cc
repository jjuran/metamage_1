/*
	Mac/Files/Functions/FlushVol.cc
	-------------------------------
*/

#include "Mac/Files/Functions/FlushVol.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Mac
{
	
	void FlushVol( ConstStr63Param volName )
	{
		Nitrogen::ThrowOSStatus( ::FlushVol( volName, 0 ) );
	}
	
	void FlushVol( FSVolumeRefNum vRefNum )
	{
		Nitrogen::ThrowOSStatus( ::FlushVol( NULL, vRefNum ) );
	}
	
}

