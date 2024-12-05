/*
	FSSpec_from_path.cc
	-------------------
*/

#include "mac_relix/FSSpec_from_path.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

// mac-relix-utils
#include "mac_relix/FSSpec_from_stat.hh"


namespace mac   {
namespace relix {

#if defined( __RELIX__ )  ||  defined( __APPLE__ )

using mac::sys::muxed_errno;
using mac::sys::muxed_OSStatus;

Error FSSpec_from_existing_path( const char* path, FSSpec& result )
{
#ifdef __RELIX__
	
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( ! nok )
	{
		nok = FSSpec_from_stat( st, result );
	}
	
	return nok ? muxed_errno( errno ) : Error();
	
#endif
	
#ifdef __APPLE__
	
	OSStatus err;
	FSRef ref;
	
	(err = FSPathMakeRef( (const UInt8*) path, &ref, NULL ))  ||
	(err = FSGetCatalogInfo( &ref, 0, 0, 0, &result, 0 ));
	
	return muxed_OSStatus( err );
	
#endif
}

Error FSSpec_from_optional_path( const char* path, FSSpec& result )
{
#ifdef __RELIX__
	
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( ! nok  ||  errno == ENOENT )
	{
		nok = FSSpec_from_stat( st, result );
	}
	
	return nok ? muxed_errno( errno ) : Error();
	
#endif
	
	return muxed_errno( ENOSYS );
}

#endif  // #if defined( __RELIX__ )  ||  defined( __APPLE__ )

}
}
