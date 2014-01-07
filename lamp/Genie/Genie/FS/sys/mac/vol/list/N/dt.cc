/*
	Genie/FS/sys/mac/vol/list/N/dt.cc
	---------------------------------
*/

#include "Genie/FS/sys/mac/vol/list/N/dt.hh"

// POSIX
#include <sys/stat.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"
#include "vfs/node/types/generated_file.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Nitrogen
{
	
	static FSSpec DTGetAPPL( FSVolumeRefNum  vRefNum,
	                         Mac::FSCreator  signature,
	                         short           index = 0 )
	{
		DTPBRec pb;
		
		PBDTGetPath( vRefNum, pb );
		
		FSSpec result;
		
		pb.ioNamePtr     = result.name;
		pb.ioIndex       = index;
		pb.ioFileCreator = signature;
		
		PBDTGetAPPLSync( pb );
		
		result.vRefNum = vRefNum;
		result.parID   = pb.ioAPPLParID;
		
		return result;
	}
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	using MacScribe::invalid_quad_name;
	using MacScribe::parse_utf8_quad_name;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const vfs::node& parent )
	{
		const vfs::node* grandparent = parent.owner();
		
		return N::FSVolumeRefNum( -gear::parse_unsigned_decimal( grandparent->name().c_str() ) );
	}
	
	
	static N::FSDirSpec DTGetInfo_Dir( N::FSVolumeRefNum vRefNum )
	{
		DTPBRec pb;
		
		N::PBDTGetPath( vRefNum, pb );
		
		Mac::ThrowOSStatus( ::PBDTGetInfoSync( &pb ) );
		
		const N::FSVolumeRefNum new_vRefNum = N::FSVolumeRefNum( pb.ioVRefNum );
		const N::FSDirID        new_dirID   = N::FSDirID       ( pb.ioDirID   );
		
		return n::make< N::FSDirSpec >( new_vRefNum, new_dirID );
	}
	
	static vfs::node_ptr desktop_dir_resolve( const vfs::node* that )
	{
		const Mac::FSVolumeRefNum vRefNum = GetKeyFromParent( *that->owner() );
		
		const N::FSDirSpec dir = DTGetInfo_Dir( vRefNum );
		
		return FSTreeFromFSDirSpec( dir );
	}
	
	static const link_method_set desktop_dir_link_methods =
	{
		NULL,
		&desktop_dir_resolve
	};
	
	static const node_method_set desktop_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&desktop_dir_link_methods
	};
	
	
	static FSSpec DTGetAPPL( const vfs::node* appls_quad, short index = 0 )
	{
		const ::OSType creator = parse_utf8_quad_name( appls_quad->name() );
		
		const vfs::node* great_x2_grandparent = appls_quad->owner()->owner()->owner();
		
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -gear::parse_unsigned_decimal( great_x2_grandparent->name().c_str() ) );
		
		return N::DTGetAPPL( vRefNum, Mac::FSCreator( creator ), index );
	}
	
	static vfs::node_ptr latest_appl_link_resolve( const vfs::node* that )
	{
		const vfs::node* parent = that->owner();
		
		const FSSpec file = DTGetAPPL( parent );
		
		return FSTreeFromFSSpec( file );
	}
	
	static const link_method_set latest_appl_link_link_methods =
	{
		NULL,
		&latest_appl_link_resolve
	};
	
	static const node_method_set latest_appl_link_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&latest_appl_link_link_methods
	};
	
	static vfs::node_ptr dt_appls_QUAD_list_N_resolve( const vfs::node* that )
	{
		const short index = gear::parse_unsigned_decimal( that->name().c_str() );
		
		const vfs::node* grandparent = that->owner()->owner();
		
		const FSSpec file = DTGetAPPL( grandparent, index );
		
		return FSTreeFromFSSpec( file );
	}
	
	static const link_method_set dt_appls_QUAD_list_N__link_methods =
	{
		NULL,
		&dt_appls_QUAD_list_N_resolve
	};
	
	static const node_method_set dt_appls_QUAD_list_N_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&dt_appls_QUAD_list_N__link_methods
	};
	
	
	static vfs::node_ptr appl_QUAD_list_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !canonical_positive_integer::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new vfs::node( parent, name, S_IFLNK | 0777, &dt_appls_QUAD_list_N_methods );
	}
	
	static void appl_QUAD_list_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		const vfs::node* grandparent = parent->owner();
		
		for ( short index = 1;  ;  ++index )
		{
			try
			{
				(void) DTGetAPPL( grandparent, index );
			}
			catch ( ... )
			{
				break;
			}
			
			cache.push_back( vfs::dir_entry( index, gear::inscribe_unsigned_decimal( index ) ) );
		}
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_appls_QUAD_latest( const vfs::node*     parent,
	                                                                  const plus::string&  name,
	                                                                  const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &latest_appl_link_methods );
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_appls_QUAD_list( const vfs::node*     parent,
	                                                                const plus::string&  name,
	                                                                const void*          args )
	{
		return new_basic_directory( parent, name, appl_QUAD_list_lookup, appl_QUAD_list_iterate );
	}
	
	extern const vfs::fixed_mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[];
	
	const vfs::fixed_mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[] =
	{
		{ "latest",  &new_sys_mac_vol_list_N_dt_appls_QUAD_latest },
		
		{ "list",  &new_sys_mac_vol_list_N_dt_appls_QUAD_list },
		
		{ NULL, NULL }
		
	};
	
	
	static void validate_quad_name( const plus::string& name )
	{
		try
		{
			(void) parse_utf8_quad_name( name );
		}
		catch ( const invalid_quad_name& )
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	static vfs::node_ptr appl_lookup( const vfs::node* parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return fixed_dir( parent, name, sys_mac_vol_list_N_dt_appls_QUAD_Mappings );
	}
	
	
	static plus::string generate_dt_icons_QUAD_QUAD_X( const vfs::node* parent, const plus::string& name )
	{
		const vfs::node*   gparent = parent ->owner();
		const vfs::node* gggparent = gparent->owner()->owner();
		
		const short selector = gear::parse_unsigned_decimal( name.c_str() );
		
		const ::OSType type    = parse_utf8_quad_name( parent ->name() );
		const ::OSType creator = parse_utf8_quad_name( gparent->name() );
		
		const short vRefNum = -gear::parse_unsigned_decimal( gggparent->name().c_str() );
		
		DTPBRec pb;
		
		N::PBDTGetPath( N::FSVolumeRefNum( vRefNum ), pb );
		
		const size_t max_icon_size = kLarge8BitIconSize;  // 1024
		
		char buffer[ max_icon_size ];
		
		pb.ioTagInfo     = 0;
		pb.ioDTBuffer      = buffer;
		pb.ioDTReqCount  = sizeof buffer;
		pb.ioIconType    = selector;
		pb.ioFileCreator = creator;
		pb.ioFileType    = type;
		
		Mac::ThrowOSStatus( ::PBDTGetIconSync( &pb ) );
		
		if ( pb.ioDTActCount > pb.ioDTReqCount )
		{
			p7::throw_errno( E2BIG );
		}
		
		plus::string result( buffer, pb.ioDTActCount );
		
		return result;
	}
	
	
	static vfs::node_ptr icon_QUAD_QUAD_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !canonical_positive_integer::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new_generated( parent, name, (void*) generate_dt_icons_QUAD_QUAD_X );
	}
	
	static void icon_QUAD_QUAD_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		const vfs::node*   gparent = parent ->owner();
		const vfs::node* gggparent = gparent->owner()->owner();
		
		const ::OSType type    = parse_utf8_quad_name( parent ->name() );
		const ::OSType creator = parse_utf8_quad_name( gparent->name() );
		
		const short vRefNum = -gear::parse_unsigned_decimal( gggparent->name().c_str() );
		
		DTPBRec pb;
		
		N::PBDTGetPath( N::FSVolumeRefNum( vRefNum ), pb );
		
		for ( short selector = 1;  ;  ++selector )
		{
			pb.ioTagInfo     = 0;
			pb.ioDTBuffer    = NULL;
			pb.ioDTReqCount  = 0;
			pb.ioIconType    = selector;
			pb.ioFileCreator = creator;
			pb.ioFileType    = type;
			
			const OSErr err = ::PBDTGetIconSync( &pb );
			
			if ( err != noErr )
			{
				break;
			}
			
			cache.push_back( vfs::dir_entry( selector, gear::inscribe_unsigned_decimal( selector ) ) );
		}
	}
	
	static vfs::node_ptr icon_QUAD_lookup( const vfs::node* parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return new_basic_directory( parent, name, icon_QUAD_QUAD_lookup, icon_QUAD_QUAD_iterate );
	}
	
	static vfs::node_ptr icon_lookup( const vfs::node* parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return new_basic_directory( parent, name, icon_QUAD_lookup, NULL );
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_dir( const vfs::node*     parent,
	                                                    const plus::string&  name,
	                                                    const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &desktop_dir_methods );
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_appls( const vfs::node*     parent,
	                                                      const plus::string&  name,
	                                                      const void*          args )
	{
		return new_basic_directory( parent, name, appl_lookup, NULL );
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_icons( const vfs::node*     parent,
	                                                      const plus::string&  name,
	                                                      const void*          args )
	{
		return new_basic_directory( parent, name, icon_lookup, NULL );
	}
	
	
	const vfs::fixed_mapping sys_mac_vol_list_N_dt_Mappings[] =
	{
		{ "dir",    &new_sys_mac_vol_list_N_dt_dir   },
		{ "appls",  &new_sys_mac_vol_list_N_dt_appls },
		{ "icons",  &new_sys_mac_vol_list_N_dt_icons },
		
		{ NULL, NULL }
		
	};
	
}

