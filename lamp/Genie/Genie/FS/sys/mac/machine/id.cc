/*
	Genie/FS/sys/mac/machine/id.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/machine/id.hh"

// plus
#include "plus/var_string.hh"

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
	
	void sys_mac_machine_id::Read( plus::var_string& result, const FSTree* that, bool binary )
	{
		const UInt32 id = GetMachineID();
		
		result = Freeze< Int_Scribe >( id, binary );
	}
	
}

