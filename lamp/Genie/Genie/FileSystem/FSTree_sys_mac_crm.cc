/*	=====================
 *	FSTree_sys_mac_crm.cc
 *	=====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_crm_serial.hh"


namespace Genie
{
	
	void FSTree_sys_mac_crm::Init()
	{
		Map( "serial", &Singleton_Factory< FSTree_sys_mac_crm_serial > );
	}
	
	FSTreePtr FSTree_sys_mac_crm::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
}

#endif

