/*	==============================
 *	FSTree_sys_mac_machine_name.cc
 *	==============================
 */

#include "Genie/FS/FSTree_sys_mac_machine_name.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static std::string GetMachineName()
	{
		const UInt32 mnam = N::Gestalt( N::GestaltSelector( gestaltUserVisibleMachineName ) );
		
		StringPtr name = reinterpret_cast< StringPtr >( mnam );
		
		return std::string( (char*) &name[1], name[0] );
	}
	
	std::string sys_mac_machine_name::Read( const FSTree* that, bool binary )
	{
		return GetMachineName();
	}
	
}

