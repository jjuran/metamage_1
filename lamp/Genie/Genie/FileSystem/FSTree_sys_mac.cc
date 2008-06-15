/*	=================
 *	FSTree_sys_mac.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_mac.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"
#include "Genie/FileSystem/FSTree_sys_mac_name.hh"
#include "Genie/FileSystem/FSTree_sys_mac_proc.hh"
#include "Genie/FileSystem/FSTree_sys_mac_rom.hh"
#include "Genie/FileSystem/FSTree_sys_mac_user.hh"
#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"


namespace Genie
{
	
	void FSTree_sys_mac::Init()
	{
		MapSingleton< FSTree_sys_mac_vol     >();
		MapSingleton< FSTree_sys_mac_proc    >();
		MapSingleton< FSTree_sys_mac_user    >();
		MapSingleton< FSTree_sys_mac_gestalt >();
		
		Map( FSTreePtr( new FSTree_QueryFile< sys_mac_name_Query >( Pathname(),
		                                                            "name" ) ) );
		
		try
		{
			GetROMSize();
			
			MapSingleton< FSTree_sys_mac_rom >();
		}
		catch ( ... )
		{
		}
	}
	
	FSTreePtr FSTree_sys_mac::Parent() const
	{
		return GetSingleton< FSTree_sys >();
	}
	
}

