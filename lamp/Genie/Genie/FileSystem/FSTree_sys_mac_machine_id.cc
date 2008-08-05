/*	===========================
 *	FSTree_sys_mac_machine_id.cc
 *	===========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_machine_id.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/Gestalt.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static std::string GetMachineID()
	{
		const UInt32 id = N::Gestalt( N::GestaltSelector( gestaltMachineType ) );
		
		return NN::Convert< std::string >( id );
	}
	
	std::string sys_mac_machine_id_Query::operator()() const
	{
		return GetMachineID() + "\n";
	}
	
}

