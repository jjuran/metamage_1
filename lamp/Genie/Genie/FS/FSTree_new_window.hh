/*	====================
 *	FSTree_new_window.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_WINDOW_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_WINDOW_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_new_window : public FSTree
	{
		public:
			FSTree_new_window( const FSTreePtr&     parent,
			                   const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			bool IsDirectory() const  { return true; }
			
			boost::shared_ptr< IOHandle > ChangeToDirectory() const;
	};
	
}

#endif

