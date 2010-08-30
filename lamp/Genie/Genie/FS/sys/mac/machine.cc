/*
	Genie/FS/sys/mac/machine.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/machine.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_sys_mac_machine_id.hh"
#include "Genie/FS/FSTree_sys_mac_machine_name.hh"


namespace Genie
{
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_machine_Mappings[] =
	{
		{ "id",   &Property_Factory< sys_mac_machine_id   > },
		{ "name", &Property_Factory< sys_mac_machine_name > },
		
		{ NULL, NULL }
	};
	
}

