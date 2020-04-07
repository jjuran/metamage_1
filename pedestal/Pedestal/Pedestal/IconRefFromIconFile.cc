/*
	IconRefFromIconFile.cc
	----------------------
*/

#include "Pedestal/IconRefFromIconFile.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	
	
	n::owned< IconRef > IconRefFromIconFile( const FSRef& file )
	{
		const OSType creator = 0;
		const OSType type    = 0;
		
		OSStatus err;
		
		IconRef icon;
		
		err = RegisterIconRefFromFSRef( creator, type, &file, &icon );
		
		if ( err != noErr )
		{
			return n::owned< IconRef >();
		}
		
		// This shouldn't fail.  If it does, something is seriously wrong.
		err = AcquireIconRef( icon );
		
		err = UnregisterIconRef( creator, type );
		
		return n::owned< IconRef >::seize( icon );
	}
	
}
