/*	=====================
 *	FSTree_sys_mac_crm.cc
 *	=====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"

// Nucleus
#include "Nucleus/ArrayContainerFunctions.h"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_crm_serial.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static FSTree_sys_mac_crm::Mapping sys_mac_crm_Mappings[] =
	{
		{ "serial", &Singleton_Factory< FSTree_sys_mac_crm_serial > }
	};
	
	void FSTree_sys_mac_crm::Init()
	{
		AddMappings( sys_mac_crm_Mappings, NN::ArrayEnd( sys_mac_crm_Mappings ) );
	}
	
}

#endif

