/*	==================
 *	FSTree_new_icon.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_ICON_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_ICON_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const Functional_Traits< void >::Mapping Icon_view_Mappings[];
	
	std::auto_ptr< Pedestal::View > IconFactory( const FSTree* delegate );
	
	
	class FSTree_new_icon : public FSTree_new_View
	{
		public:
			FSTree_new_icon( const FSTreePtr&    parent,
			                 const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &IconFactory,
				                 Icon_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

