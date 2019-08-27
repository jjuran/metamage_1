/*
	directory.hh
	------------
*/

#ifndef MACFILE_DIRECTORY_HH
#define MACFILE_DIRECTORY_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

struct FSRef;
struct FSSpec;


namespace mac  {
namespace file {
	
	inline
	const types::VRefNum_DirID& directory( const types::VRefNum_DirID& dir )
	{
		return dir;
	}
	
	types::VRefNum_DirID directory( const FSSpec& file );
	types::VRefNum_DirID directory( const FSRef&  file );
	
}
}

#endif
