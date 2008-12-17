/*	====================
 *	FSTree_new_caption.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_CAPTION_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_CAPTION_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const Functional_Traits< void >::Mapping Caption_view_Mappings[];
	
	std::auto_ptr< Pedestal::View > CaptionFactory( const FSTree* delegate );
	
	
	class FSTree_new_caption : public FSTree_new_View
	{
		public:
			FSTree_new_caption( const FSTreePtr&    parent,
			                    const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &CaptionFactory,
				                 Caption_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

