/*	=========================
 *	FSTree_new_scrollframe.hh
 *	=========================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_SCROLLFRAME_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_SCROLLFRAME_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping ScrollFrame_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > ScrollFrameFactory( const FSTree* delegate );
	
	
	class FSTree_new_scrollframe : public FSTree_new_View
	{
		public:
			FSTree_new_scrollframe( const FSTreePtr&    parent,
			                        const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &ScrollFrameFactory,
				                 ScrollFrame_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

