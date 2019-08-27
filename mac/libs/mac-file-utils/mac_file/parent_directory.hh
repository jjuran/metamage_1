/*
	parent_directory.hh
	-------------------
*/

#ifndef MACFILE_PARENTDIRECTORY_HH
#define MACFILE_PARENTDIRECTORY_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

struct FSRef;
struct FSSpec;


namespace mac  {
namespace file {
	
	inline
	const types::VRefNum_DirID& parent_directory( const FSSpec& file )
	{
		return (const types::VRefNum_DirID&) file;
	}
	
	types::VRefNum_DirID parent_directory( const types::VRefNum_DirID& dir );
	
	types::VRefNum_DirID parent_directory( const FSRef& file );
	
}
}

#endif
