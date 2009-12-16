/*
	SymbolicLink.hh
	---------------
*/

#ifndef GENIE_FS_SYMBOLICLINK_HH
#define GENIE_FS_SYMBOLICLINK_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&    parent,
	                                   const std::string&  name,
	                                   const std::string&  target );
	
}

#endif

