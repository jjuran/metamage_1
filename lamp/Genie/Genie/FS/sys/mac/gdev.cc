/*
	Genie/FS/sys/mac/gdev.cc
	------------------------
*/

#include "Genie/FS/sys/mac/gdev.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Genie
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/sys/mac/gdev/list.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class sys_mac_gdev_main : public FSTree_ReadableSymLink
	{
		public:
			sys_mac_gdev_main( const FSTreePtr&     parent,
			                   const plus::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
				if ( !MacFeatures::Has_ColorQuickdraw() )
				{
					p7::throw_errno( ENOENT );
				}
			}
			
			plus::string ReadLink() const;
	};
	
	plus::string sys_mac_gdev_main::ReadLink() const
	{
		const GDHandle gdH = ::GetMainDevice();
		
		ASSERT( gdH != NULL );
		
		plus::var_string result = "list/12345678";
		
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

