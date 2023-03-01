/*
	open_res_file.cc
	----------------
*/

#include "mac_rsrc/open_res_file.hh"

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


namespace mac  {
namespace rsrc {

ResFileRefNum open_res_file( const FSSpec& file, signed char perm )
{
	ResFileRefNum result;
	
#if ! __LP64__
	
	if ( mac::sys::has_FSSpec_calls() )
	{
		result = FSpOpenResFile( &file, perm );
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
		
		result = OpenResFile( file.name );
		
	#endif
		
		HSetVol( NULL, saved_vRefNum, saved_dirID );
	}
	
	if ( OSErr err = mac::sys::res_error() )
	{
		return err;
	}
	
	return result < 0 ? resFNotFound : result;
	
#endif
	
	return 0;
}

ResFileRefNum open_res_file( const FSRef& file, signed char perm )
{
	ResFileRefNum result = FSOpenResFile( &file, perm );
	
	OSErr err = mac::sys::res_error();
	
	return err ? err : result;
}

}
}
