/*
	Mac/Files/Functions/UnmountVol.cc
	---------------------------------
*/

#include "Mac/Files/Functions/UnmountVol.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Mac
{
	
#if ! __LP64__
	
	void UnmountVol( ConstStr63Param volName )
	{
		ThrowOSStatus( ::UnmountVol( volName, 0 ) );
	}
	
	void UnmountVol( FSVolumeRefNum vRefNum )
	{
		ThrowOSStatus( ::UnmountVol( NULL, vRefNum ) );
	}
	
#else
	
	void dummy()
	{
	}
	
#endif
	
}
