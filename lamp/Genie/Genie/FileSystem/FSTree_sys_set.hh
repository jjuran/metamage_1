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
			FSTree_sys_set( const FSTreePtr& parent ) : FSTree_Functional_Singleton( parent )
			{
			}
			
			void Init();
			
			static std::string OnlyName()  { return "set"; }
			
			std::string Name() const  { return OnlyName(); }
	};
	
}

#endif

