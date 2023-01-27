/*
	open_rsrc_fork.hh
	-----------------
*/

#ifndef MACFILE_OPENRSRCFORK_HH
#define MACFILE_OPENRSRCFORK_HH

// mac-file-utils
#include "mac_file/FSIORefNum.hh"


struct FSRef;
struct FSSpec;

namespace mac  {
namespace file {
	
	FSIORefNum open_rsrc_fork( const FSSpec& file, signed char perm );
	FSIORefNum open_rsrc_fork( const FSRef&  file, signed char perm );
	
}
}

#endif
