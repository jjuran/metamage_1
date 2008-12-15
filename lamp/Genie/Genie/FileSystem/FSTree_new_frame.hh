/*	===================
 *	FSTree_new_frame.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_FRAME_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_FRAME_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_new_frame : public FSTree
	{
		public:
			FSTree_new_frame( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			void HardLink( const FSTreePtr& target ) const;
	};
	
}

#endif

