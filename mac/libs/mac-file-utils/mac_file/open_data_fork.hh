/*
	open_data_fork.hh
	-----------------
*/

#ifndef MACFILE_OPENDATAFORK_HH
#define MACFILE_OPENDATAFORK_HH

struct FSRef;
struct FSSpec;

namespace mac  {
namespace file {
	
#if __LP64__
	
	typedef int FSIORefNum;
	
#else
	
	typedef short FSIORefNum;
	
#endif
	
	FSIORefNum open_data_fork( const FSSpec& file, signed char perm );
	FSIORefNum open_data_fork( const FSRef&  file, signed char perm );
	
}
}

#endif
