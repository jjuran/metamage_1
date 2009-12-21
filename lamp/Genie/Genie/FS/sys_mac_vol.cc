/*
	sys_mac_vol.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol.hh"

// iota
#include "iota/decimal.hh"

// Nitrogen
#include "Nitrogen/Folders.h"

// Genie
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/sys_mac_vol_list.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class sys_mac_vol_boot : public FSTree_ReadableSymLink
	{
		public:
			sys_mac_vol_boot( const FSTreePtr&    parent,
			                  const std::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			std::string ReadLink() const;
	};
	
	std::string sys_mac_vol_boot::ReadLink() const
	{
		const N::FSDirSpec system_folder = N::FindFolder( N::kOnAppropriateDisk,
		                                                  N::kSystemFolderType,
		                                                  false );
		
		const N::FSVolumeRefNum vRefNum = system_folder.vRefNum;
		
		std::string result = "list/";
		
		result += iota::inscribe_decimal( -vRefNum );
		
		return result;
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ "boot", &Basic_Factory< sys_mac_vol_boot > },
		
		{ NULL, NULL }
		
	};
	
}

