/*	=======================
 *	FSTree_new_iconsuite.hh
 *	=======================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_ICONSUITE_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_ICONSUITE_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping IconSuite_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > IconSuiteFactory( const FSTree* delegate );
	
	
	class FSTree_new_iconsuite : public FSTree_new_View
	{
		public:
			FSTree_new_iconsuite( const FSTreePtr&    parent,
			                      const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &IconSuiteFactory,
				                 IconSuite_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

