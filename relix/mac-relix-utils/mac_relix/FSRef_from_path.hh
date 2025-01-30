/*
	FSRef_from_path.hh
	------------------
*/

#ifndef MACRELIX_FSREFFROMPATH_HH
#define MACRELIX_FSREFFROMPATH_HH

// mac-sys-utils
#include "mac_sys/error.hh"


struct FSRef;


namespace mac   {
namespace relix {
	
	using mac::sys::Error;
	
	Error FSRef_from_path( const char* path, FSRef& result );
	
	inline
	Error FSObj_from_existing_path( const char* path, FSRef& result )
	{
		return FSRef_from_path( path, result );
	}
	
}
}

#endif
