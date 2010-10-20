/*
	Genie/FS/sys/mac/machine/id.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/machine/id.hh"

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	typedef plus::serialize_unsigned< UInt32 >::deconstruct deconstruct_id;
	
	
	static inline UInt32 GetMachineID()
	{
		const UInt32 id = N::Gestalt( N::GestaltSelector( gestaltMachineType ) );
		
		return id;
	}
	
	void sys_mac_machine_id::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		const UInt32 id = GetMachineID();
		
		deconstruct_id::apply( result, id, binary );
	}
	
}

