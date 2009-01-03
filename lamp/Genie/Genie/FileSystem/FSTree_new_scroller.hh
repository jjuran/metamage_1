/*	======================
 *	FSTree_new_scroller.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_SCROLLER_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_SCROLLER_HH

// Pedestal
//#include "Pedestal/Scroller_beta.hh"

// Genie
#include "Genie/FileSystem/TrackScrollbar.hh"
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping Scroller_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > BasicScrollerFactory( const FSTree* delegate );
	
	
	class FSTree_new_scroller : public FSTree_new_View
	{
		public:
			FSTree_new_scroller( const FSTreePtr&    parent,
			                     const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &BasicScrollerFactory,
				                 Scroller_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			FSTreePtr CreateDelegate( const FSTreePtr&    parent,
			                          const std::string&  name ) const;
		
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

