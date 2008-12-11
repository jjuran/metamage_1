/*	====================
 *	FSTree_new_caption.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_CAPTION_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_CAPTION_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_new_caption : public FSTree
	{
		public:
			FSTree_new_caption( const FSTreePtr&    parent,
			                    const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			void HardLink( const FSTreePtr& target ) const;
	};
	
}

#endif

