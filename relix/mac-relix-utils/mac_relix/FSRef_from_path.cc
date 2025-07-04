/*
	FSRef_from_path.cc
	------------------
*/

#include "mac_relix/FSRef_from_path.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


namespace mac   {
namespace relix {

#if defined( __RELIX__ )  ||  defined( __APPLE__ )

Error FSRef_from_path( const char* path, FSRef& result )
{
#ifdef __APPLE__
	
	return (Error) FSPathMakeRef( (const UInt8*) path, &result, NULL );
	
#endif
	
#ifdef __RELIX__
	
	FSSpec spec;
	
	Error err = FSSpec_from_existing_path( path, spec );
	
	if ( ! err )
	{
		err = (Error) FSpMakeFSRef( &spec, &result );
	}
	
	return err;
	
#endif
}

#endif  // #if defined( __RELIX__ )  ||  defined( __APPLE__ )

}
}
