/*	===================
 *	FSTree_new_frame.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_FRAME_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_FRAME_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	class FSTree_new_frame : public FSTree_new_View
	{
		public:
			FSTree_new_frame( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree_new_View( parent, name )
			{
			}
			
			FSTreePtr MakeDelegate( const FSTreePtr& parent, const std::string& name ) const;
			
			ViewFactory GetViewFactory() const;
	};
	
}

#endif

