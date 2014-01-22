/*
	Genie/FS/sys/mac/vol.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/vol.hh"

// Standard C++
#include <algorithm>

// mac-sys-utils
#include "mac_sys/unit_table.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Folders.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/symbolic_link.hh"

// Genie
#include "Genie/FS/sys/mac/vol/list.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static Mac::FSVolumeRefNum find_boot_disk()
	{
		const N::FSDirSpec system_folder = N::FindFolder( N::kOnSystemDisk,
		                                                  N::kSystemFolderType,
		                                                  false );
		
		return system_folder.vRefNum;
	}
	
	
	struct volume_is_ram_disk
	{
		static bool applies( N::FSVolumeRefNum vRefNum );
		
		bool operator()( N::FSVolumeRefNum vRefNum ) const
		{
			return applies( vRefNum );
		}
	};
	
	static void PBHGetVInfoSync( HVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
	{
		pb.ioNamePtr  = name;
		pb.ioVRefNum  = vRefNum;
		pb.filler2    = 0;
		pb.ioVolIndex = 0;
		
		Mac::ThrowOSStatus( ::PBHGetVInfoSync( (HParamBlockRec*) &pb ) );
	}
	
	bool volume_is_ram_disk::applies( N::FSVolumeRefNum vRefNum )
	{
		using mac::types::AuxDCE;
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, vRefNum );
		
		if ( pb.ioVDRefNum == 0 )
		{
			return false;
		}
		
		const SInt16 unit_number = ~pb.ioVDRefNum;
		
		AuxDCE** const dceHandle = mac::sys::get_unit_table_base()[ unit_number ];
		
		const unsigned char* name = mac::sys::get_driver_name( dceHandle );
		
		return std::equal( name,
		                   name + 1 + name[0],
		                   "\p" ".EDisk" );
		
		return false;
	}
	
	static N::FSVolumeRefNum find_ram_disk()
	{
		typedef N::Volume_Container::const_iterator iterator;
		
		N::Volume_Container sequence = N::Volumes();
		
		const iterator it = std::find_if( sequence.begin(),
		                                  sequence.end(),
		                                  volume_is_ram_disk() );
		
		return it != sequence.end() ? *it : N::FSVolumeRefNum( 0 );
	}
	
	
	static vfs::node_ptr new_volume_link( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		typedef Mac::FSVolumeRefNum (*Function)();
		
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
		
		{ "boot", &new_volume_link, (void*) &find_boot_disk },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "ram",  &new_volume_link, (void*) &find_ram_disk },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}

