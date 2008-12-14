/*	=========================
 *	FSTree_sys_mac_machine.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_machine.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_machine_id.hh"
#include "Genie/FileSystem/FSTree_sys_mac_machine_name.hh"


namespace Genie
{
	
	template < class Query >
	static FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_mac_machine_Mappings[] =
	{
		{ "id",   &Query_Factory< sys_mac_machine_id_Query > },
		{ "name", &Query_Factory< sys_mac_machine_name_Query > },
		
		{ NULL, NULL }
	};
	
}

