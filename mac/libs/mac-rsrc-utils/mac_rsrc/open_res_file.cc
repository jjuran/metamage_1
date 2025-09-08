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


#pragma exceptions off


namespace mac  {
namespace rsrc {

ResFileRefNum open_res_file( short        vRefNum,
                             long         parID,
                             const Byte*  file_name,
                             signed char  perm )
{
	ResFileRefNum result;
	
#if ! __LP64__
	
	if ( mac::sys::has_FSSpec_calls() )
	{
		result = HOpenResFile( vRefNum, parID, file_name, perm );
	}
	else
	{
		OSErr err;
		Str27 name;
		short saved_vRefNum;
		long  saved_dirID;
		
		(err = HGetVol( name, &saved_vRefNum, &saved_dirID ))  ||
		(err = HSetVol( NULL, vRefNum,        parID        ));
		
		if ( err )
		{
			return err;
		}
		
	#if CALL_NOT_IN_CARBON
		
		result = OpenResFile( file_name );
		
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

#if ! __LP64__

ResFileRefNum open_res_file( const FSSpec& file, signed char perm )
{
	return open_res_file( file.vRefNum, file.parID, file.name, perm );
}

#endif

ResFileRefNum open_res_file( const FSRef& file, signed char perm )
{
	ResFileRefNum result = FSOpenResFile( &file, perm );
	
	OSErr err = mac::sys::res_error();
	
	return err ? err : result;
}

ResFileRefNum open_res_file( const FSRef&         file,
                             const HFSUniStr255&  forkName,
                             signed char          perm )
{
	ResFileRefNum result;
	
	UInt16         n = forkName.length;
	const UniChar* s = forkName.unicode;
	
	OSErr err = FSOpenResourceFile( &file, n, s, perm, &result );
	
	return err ? err : result;
}

}
}
