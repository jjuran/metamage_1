/*	======================
 *	FSTree_sys_mac_user.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUSER_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUSER_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys_mac_user : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys_mac_user( const FSTreePtr& parent ) : FSTree_Functional_Singleton( parent )
			{
			}
			
			void Init();
			
			static std::string OnlyName()  { return "user"; }
			
			std::string Name() const  { return OnlyName(); }
	};
	
}

#endif

