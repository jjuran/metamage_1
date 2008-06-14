/*	===========================
 *	FSTree_sys_mac_user_name.cc
 *	===========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"

// Nitrogen
//#include "Nitrogen/Folders.h"


namespace Genie
{
	
	//namespace N = Nitrogen;
	
	
	std::string sys_mac_user_name_Query::operator()() const
	{
		return "J. Random User";
	}
	
}

