/*	=================
 *	FSTree_sys_mac.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMAC_HH
#define GENIE_FILESYSTEM_FSTREESYSMAC_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys_mac : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys_mac( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
}

#endif

