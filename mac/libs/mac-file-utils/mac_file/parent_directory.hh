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
	
	using types::VRefNum_DirID;
	
	inline
	const VRefNum_DirID& parent_directory( const FSSpec& file )
	{
		return (const VRefNum_DirID&) file;
	}
	
	VRefNum_DirID parent_directory( const VRefNum_DirID& dir );
	
	VRefNum_DirID parent_directory( const FSRef& file );
	
}
}

#endif
