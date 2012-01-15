/*
	CreatableSymLink.hh
	-------------------
*/

#ifndef GENIE_FS_CREATABLESYMLINK_HH
#define GENIE_FS_CREATABLESYMLINK_HH

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	typedef void (*symlinker)( const FSTree*        parent,
	                           const plus::string&  name,
	                           const plus::string&  target );
	
	FSTreePtr New_CreatableSymLink( const FSTreePtr&     parent,
	                                const plus::string&  name,
	                                symlinker            symlink );
	
}

#endif

