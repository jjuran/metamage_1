/*
	OpenDataFork.cc
	---------------
*/

#include "Genie/Utilities/OpenDataFork.hh"

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Genie
{
	
	nucleus::owned< Mac::FSFileRefNum > OpenDataFork( const FSSpec&  file,
	                                                  Mac::FSIOPerm  perm )
	{
		OSErr err;
		short result;
		
		if ( mac::sys::has_FSSpec_calls() )
		{
			err = FSpOpenDF( &file, perm, &result );
		}
		else
		{
			err = HOpen( file.vRefNum, file.parID, file.name, perm, &result );
		}
		
		Mac::ThrowOSStatus( err );
		
		using Mac::FSFileRefNum;
		
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
	}
	
}
