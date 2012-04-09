/*
	SymbolicLink.hh
	---------------
*/

#ifndef GENIE_FS_SYMBOLICLINK_HH
#define GENIE_FS_SYMBOLICLINK_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	class FSTree_SymbolicLink : public FSTree
	{
		public:
			FSTree_SymbolicLink( const FSTreePtr&     parent,
			                     const plus::string&  name,
			                     plus::string         target,
			                     remove_method        remove = NULL );
	};
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   plus::string         target,
	                                   remove_method        remove = NULL );
	
}

#endif

