/*
	HFS_pathname.hh
	---------------
*/

#ifndef MACFILE_HFSPATHNAME_HH
#define MACFILE_HFSPATHNAME_HH

// #include <Files.h>
struct FSSpec;


namespace mac  {
namespace file {
	
	long HFS_pathname( char* buffer, unsigned len, const FSSpec& file );
	
}
}

#endif
