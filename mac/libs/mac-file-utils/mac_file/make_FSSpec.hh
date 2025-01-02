/*
	make_FSSpec.hh
	--------------
*/

#ifndef MACFILE_MAKEFSSPEC_HH
#define MACFILE_MAKEFSSPEC_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

struct FSSpec;

typedef short OSErr;


namespace mac  {
namespace file {
	
	using types::VRefNum_DirID;
	
	OSErr make_FSSpec( FSSpec& result, const VRefNum_DirID& dir );
	
}
}

#endif
