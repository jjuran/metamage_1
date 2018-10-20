/*
	Mac/Files/Functions/FlushVol.cc
	-------------------------------
*/

#include "Mac/Files/Functions/FlushVol.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Mac
{
	
#if ! __LP64__
	
	void FlushVol( ConstStr63Param volName )
	{
		ThrowOSStatus( ::FlushVol( volName, 0 ) );
	}
	
	void FlushVol( FSVolumeRefNum vRefNum )
	{
		ThrowOSStatus( ::FlushVol( NULL, vRefNum ) );
	}
	
#else
	
	void dummy()
	{
	}
	
#endif
	
}
