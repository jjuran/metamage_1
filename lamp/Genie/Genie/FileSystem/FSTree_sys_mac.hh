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
	
	class FSTree_sys_mac : public FSTree_Virtual
	{
		public:
			void Init();
			
			static std::string OnlyName()  { return "mac"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const;
	};
	
}

#endif

