/*	===========================
 *	FSTree_sys_mac_machine_id.cc
 *	===========================
 */

#include "Genie/FS/FSTree_sys_mac_machine_id.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"

// Genie
#include "Genie/FS/Scribes.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static inline UInt32 GetMachineID()
	{
		const UInt32 id = N::Gestalt( N::GestaltSelector( gestaltMachineType ) );
		
		return id;
	}
	
	plus::string sys_mac_machine_id::Read( const FSTree* that, bool binary )
	{
		const UInt32 id = GetMachineID();
		
		return Freeze< Int_Scribe >( id, binary );
	}
	
}

