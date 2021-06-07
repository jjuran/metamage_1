/*
	open_data_fork.hh
	-----------------
*/

#ifndef MACFILE_OPENDATAFORK_HH
#define MACFILE_OPENDATAFORK_HH

struct FSSpec;

namespace mac  {
namespace file {
	
	short open_data_fork( const FSSpec& file, signed char perm );
	
}
}

#endif
