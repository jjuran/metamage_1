/*	====================
 *	FSTree_new_button.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_BUTTON_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_BUTTON_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping Button_view_Mappings[];
	
	
	class FSTree_new_button : public FSTree_new_View
	{
		public:
			FSTree_new_button( const FSTreePtr&    parent,
			                   const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &Factory,
				                 Button_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static boost::shared_ptr< Pedestal::View >
			//
			Factory( const FSTree* delegate );
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

