/*	=========================
 *	FSTree_sys_mac_machine.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_machine.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_mac_machine_id.hh"
#include "Genie/FileSystem/FSTree_sys_mac_machine_name.hh"


namespace Genie
{
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	const Singleton_Mapping sys_mac_machine_Mappings[] =
	{
		{ "id",   &Property_Factory< sys_mac_machine_id   > },
		{ "name", &Property_Factory< sys_mac_machine_name > },
		
		{ NULL, NULL }
	};
	
}

