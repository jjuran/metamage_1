/*	==============================
 *	FSTree_sys_mac_machine_name.cc
 *	==============================
 */

#include "Genie/FileSystem/FSTree_sys_mac_machine_name.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/Gestalt.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static std::string GetMachineName()
	{
		const UInt32 mnam = N::Gestalt( N::GestaltSelector( gestaltUserVisibleMachineName ) );
		
		StringPtr name = reinterpret_cast< StringPtr >( mnam );
		
		return NN::Convert< std::string >( name );
	}
	
	std::string sys_mac_machine_name::Read( const FSTree* that, bool binary )
	{
		return GetMachineName();
	}
	
}

