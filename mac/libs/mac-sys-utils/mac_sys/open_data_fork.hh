/*
	open_data_fork.hh
	-----------------
*/

#ifndef MACSYS_OPENDATAFORK_HH
#define MACSYS_OPENDATAFORK_HH

struct FSSpec;

namespace mac {
namespace sys {
	
	short open_data_fork( const FSSpec& file, signed char perm );
	
}
}

#endif
