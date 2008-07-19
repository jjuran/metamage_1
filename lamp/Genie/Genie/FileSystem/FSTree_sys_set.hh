/*	=================
 *	FSTree_sys_set.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSSET_HH
#define GENIE_FILESYSTEM_FSTREESYSSET_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys_set : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys_set( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
}

#endif

