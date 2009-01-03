/*	======================
 *	FSTree_new_textedit.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_TEXTEDIT_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_TEXTEDIT_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping TextEdit_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > TextEditFactory( const FSTree* delegate );
	
	
	class FSTree_new_textedit : public FSTree_new_View
	{
		public:
			FSTree_new_textedit( const FSTreePtr&    parent,
			                     const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &TextEditFactory,
				                 TextEdit_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

