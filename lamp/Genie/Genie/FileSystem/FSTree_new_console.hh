/*	=====================
 *	FSTree_new_console.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_CONSOLE_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_CONSOLE_HH

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping Console_view_Mappings[];
	
	boost::shared_ptr< Pedestal::View > ConsoleFactory( const FSTree* delegate );
	
	
	class FSTree_new_console : public FSTree_new_View
	{
		public:
			FSTree_new_console( const FSTreePtr&    parent,
			                    const std::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &ConsoleFactory,
				                 Console_view_Mappings,
				                 &DestroyDelegate )
			{
			}
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

