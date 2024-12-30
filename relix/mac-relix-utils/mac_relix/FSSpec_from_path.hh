/*
	FSSpec_from_path.hh
	-------------------
*/

#ifndef MACRELIX_FSSPECFROMPATH_HH
#define MACRELIX_FSSPECFROMPATH_HH

// mac-sys-utils
#include "mac_sys/error.hh"


struct FSSpec;


namespace mac   {
namespace relix {
	
	using mac::sys::Error;
	
	Error FSSpec_from_existing_path( const char* path, FSSpec& result );
	
	inline
	Error FSObj_from_existing_path( const char* path, FSSpec& result )
	{
		return FSSpec_from_existing_path( path, result );
	}
	
}
}

#endif
