/*	======================
 *	FSTree_sys_mac_gdev.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_gdev.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// Genie
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/sys_mac_gdev_list.hh"


namespace Genie
{
	
	class sys_mac_gdev_main : public FSTree_ReadableSymLink
	{
		public:
			sys_mac_gdev_main( const FSTreePtr&    parent,
			                   const std::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			std::string ReadLink() const;
	};
	
	std::string sys_mac_gdev_main::ReadLink() const
	{
		const GDHandle gdH = ::GetMainDevice();
		
		ASSERT( gdH != NULL );
		
		std::string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		iota::encode_32_bit_hex( (unsigned) gdH, &result[ hex_offset ] );
		
		return result;
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_gdev_Mappings[] =
	{
		{ "main", &Basic_Factory< sys_mac_gdev_main > },
		
		{ "list", &New_FSTree_sys_mac_gdev_list },
		
		{ NULL, NULL }
		
	};
	
}

