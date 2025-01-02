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
	
	using types::VRefNum_DirID;
	
	inline
	const VRefNum_DirID& directory( const VRefNum_DirID& dir )
	{
		return dir;
	}
	
	VRefNum_DirID directory( const FSSpec& file );
	VRefNum_DirID directory( const FSRef&  file );
	
}
}

#endif
