/*
	create_res_file.cc
	------------------
*/

#include "mac_rsrc/create_res_file.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"
#include "mac_sys/res_error.hh"


#pragma exceptions off


namespace mac  {
namespace rsrc {

OSErr create_res_file( const FSSpec& file )
{
#if ! __LP64__
	
	if ( mac::sys::has_FSSpec_calls() )
	{
		/*
			HCreateResFile() is present in System 7, but not System 6, so
			presumably it's in the set of "FSSpec calls" and would therefore
			also be available when QuickTime is installed.
			
			We use HCreateResFile() instead of FSpCreateResFile() because it's
			closer to CreateResFile(), in that it doesn't take creator, type,
			and script as parameters -- let the caller deal with that, since
			it has to for CreateResFile() anyway.
		*/
		
		HCreateResFile( file.vRefNum, file.parID, file.name );
	}
	else
	{
		OSErr err;
		Str27 name;
		short saved_vRefNum;
		long  saved_dirID;
		
		(err = HGetVol( name, &saved_vRefNum, &saved_dirID ))  ||
		(err = HSetVol( NULL, file.vRefNum,   file.parID   ));
		
		if ( err )
		{
			return err;
		}
		
	#if CALL_NOT_IN_CARBON
		
		CreateResFile( file.name );
		
	#endif
		
		HSetVol( NULL, saved_vRefNum, saved_dirID );
	}
	
	return mac::sys::res_error();
	
#endif
	
	return 0;
}

}
}
