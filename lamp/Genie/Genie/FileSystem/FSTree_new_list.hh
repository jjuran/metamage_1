/*	==================
 *	FSTree_new_list.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_LIST_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_LIST_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping List_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > ListFactory( const FSTree* delegate );
	
	
	class FSTree_new_list : public FSTree_new_View
	{
		public:
			FSTree_new_list( const FSTreePtr&    parent,
			                 const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &ListFactory,
				                 List_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

