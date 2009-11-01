/*
	ResolvableSymLink.hh
	--------------------
*/

#ifndef GENIE_FS_RESOLVABLESYMLINK_HH
#define GENIE_FS_RESOLVABLESYMLINK_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_ResolvableSymLink : public FSTree
	{
		public:
			FSTree_ResolvableSymLink( const FSTreePtr&    parent,
			                          const std::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const;
	};
	
}

#endif

