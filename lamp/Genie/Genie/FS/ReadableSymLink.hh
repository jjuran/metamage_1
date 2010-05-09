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
	
	class FSTree_ReadableSymLink : public FSTree
	{
		public:
			FSTree_ReadableSymLink( const FSTreePtr&     parent,
			                        const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			FSTreePtr ResolveLink() const;
	};
	
}

#endif

