/*	====================
 *	FSTree_new_iconid.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_ICONID_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_ICONID_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping IconID_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > IconIDFactory( const FSTree* delegate );
	
	
	class FSTree_new_iconid : public FSTree_new_View
	{
		public:
			FSTree_new_iconid( const FSTreePtr&    parent,
			                   const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &IconIDFactory,
				                 IconID_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

