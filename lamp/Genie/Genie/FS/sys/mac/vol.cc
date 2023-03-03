/*
	Genie/FS/sys/mac/vol.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/vol.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-sys-utils
#include "mac_sys/unit_table.hh"

// mac-file-utils
#include "mac_file/boot_volume.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/symbolic_link.hh"

// Genie
#include "Genie/FS/sys/mac/vol/list.hh"


#define PSTR_LEN( s )  "\p" s, sizeof s


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static short find_ram_disk()
	{
		using mac::types::AuxDCE;
		
		AuxDCE*** const unit_table = mac::sys::get_unit_table_base();
		
		HParamBlockRec pb;
		
		pb.volumeParam.ioNamePtr = NULL;
		
		for ( short index = 1;  ;  ++index )
		{
			pb.volumeParam.ioVRefNum  = 0;
			pb.volumeParam.ioVolIndex = index;
			
			if ( OSErr err = ::PBHGetVInfoSync( &pb ) )
			{
				if ( err == nsvErr )
				{
					break;
				}
				
				// Unexpected error
				break;
			}
			
			if ( const short dRefNum = pb.volumeParam.ioVDRefNum )
			{
				const short unit_number = ~dRefNum;
				
				AuxDCE** const dceHandle = unit_table[ unit_number ];
				
				const unsigned char* name = mac::sys::get_driver_name( dceHandle );
				
				if ( memcmp( name, PSTR_LEN( ".EDisk" ) ) == 0 )
				{
					return pb.volumeParam.ioVRefNum;
				}
			}
		}
		
		return 0;
	}
	
	
	static vfs::node_ptr new_volume_link( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		typedef short (*Function)();
		
		Function f = (Function) args;
		
		const SInt16 vRefNum = f();
		
		if ( vRefNum == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::var_string result = "list/";
		
		result += gear::inscribe_decimal( -vRefNum );
		
		return vfs::new_symbolic_link( parent, name, result );
		
	}
	
	const vfs::fixed_mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ "boot", &new_volume_link, (void*) &mac::file::boot_volume },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "ram",  &new_volume_link, (void*) &find_ram_disk },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}
