/*
	ReadableSymLink.hh
	------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_READABLESYMLINK_HH
#define GENIE_FS_READABLESYMLINK_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	struct node_method_set;
	
	
	class FSTree_ReadableSymLink : public FSTree
	{
		public:
			FSTree_ReadableSymLink( const FSTreePtr&        parent,
			                        const plus::string&     name,
			                        const node_method_set*  methods = NULL );
			
			FSTreePtr ResolveLink() const;
	};
	
}

#endif

