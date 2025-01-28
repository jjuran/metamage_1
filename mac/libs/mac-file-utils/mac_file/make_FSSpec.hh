/*
	make_FSSpec.hh
	--------------
*/

#ifndef MACFILE_MAKEFSSPEC_HH
#define MACFILE_MAKEFSSPEC_HH

// mac-types
#include "mac_types/FSSpec.hh"
#include "mac_types/VRefNum_DirID.hh"

struct FSSpec;

typedef short OSErr;


namespace mac  {
namespace file {
	
	OSErr make_FSSpec( FSSpec& result, const types::VRefNum_DirID& dir );
	
	types::FSSpec make_FSSpec( const types::VRefNum_DirID& dir );
	
}
}

#endif
