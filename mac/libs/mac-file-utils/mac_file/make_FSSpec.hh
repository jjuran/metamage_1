/*
	make_FSSpec.hh
	--------------
*/

#ifndef MACFILE_MAKEFSSPEC_HH
#define MACFILE_MAKEFSSPEC_HH

// mac-types
#include "mac_types/FSSpec.hh"
#include "mac_types/VRefNum_DirID.hh"


namespace mac  {
namespace file {
	
	types::FSSpec make_FSSpec( const types::VRefNum_DirID& dir );
	
}
}

#endif
