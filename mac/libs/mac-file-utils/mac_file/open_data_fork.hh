/*
	open_data_fork.hh
	-----------------
*/

#ifndef MACFILE_OPENDATAFORK_HH
#define MACFILE_OPENDATAFORK_HH

// mac-file-utils
#include "mac_file/FSIORefNum.hh"


struct FSRef;
struct FSSpec;

namespace mac  {
namespace file {
	
	FSIORefNum open_data_fork( const FSSpec& file, signed char perm );
	FSIORefNum open_data_fork( const FSRef&  file, signed char perm );
	
}
}

#endif
