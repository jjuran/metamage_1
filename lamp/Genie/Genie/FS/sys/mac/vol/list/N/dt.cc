/*
	Genie/FS/sys/mac/vol/list/N/dt.cc
	---------------------------------
*/

#include "Genie/FS/sys/mac/vol/list/N/dt.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Generated.hh"
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
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		const FSTreePtr& grandparent = parent->ParentRef();
		
		return N::FSVolumeRefNum( -gear::parse_unsigned_decimal( grandparent->Name().c_str() ) );
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
	
	static FSTreePtr desktop_dir_resolve( const FSTree* node )
	{
		const Mac::FSVolumeRefNum vRefNum = GetKeyFromParent( node->owner() );
		
		const N::FSDirSpec dir = DTGetInfo_Dir( vRefNum );
		
		const bool onServer = VolumeIsOnServer( vRefNum );
		
		return FSTreeFromFSDirSpec( dir, onServer );
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
	
	
	static FSSpec DTGetAPPL( const FSTree* appls_quad, short index = 0 )
	{
		const ::OSType creator = parse_utf8_quad_name( appls_quad->Name() );
		
		const FSTreePtr& great_x2_grandparent = appls_quad->ParentRef()->ParentRef()->ParentRef();
		
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -gear::parse_unsigned_decimal( great_x2_grandparent->Name().c_str() ) );
		
		return N::DTGetAPPL( vRefNum, Mac::FSCreator( creator ), index );
	}
	
	static FSTreePtr latest_appl_link_resolve( const FSTree* node )
	{
		const FSTree* parent = node->owner();
		
		const FSSpec file = DTGetAPPL( parent );
		
		const bool onServer = VolumeIsOnServer( N::FSVolumeRefNum( file.vRefNum ) );
		
		return FSTreeFromFSSpec( file, onServer );
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
	
	static FSTreePtr dt_appls_QUAD_list_N_resolve( const FSTree* node )
	{
		const short index = gear::parse_unsigned_decimal( node->name().c_str() );
		
		const FSTree* grandparent = node->owner()->owner();
		
		const FSSpec file = DTGetAPPL( grandparent, index );
		
		const bool onServer = VolumeIsOnServer( N::FSVolumeRefNum( file.vRefNum ) );
		
		return FSTreeFromFSSpec( file, onServer );
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
	
	
	static FSTreePtr appl_QUAD_list_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !canonical_positive_integer::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new FSTree( parent, name, S_IFLNK | 0777, &dt_appls_QUAD_list_N_methods );
	}
	
	static void appl_QUAD_list_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		const FSTree* grandparent = parent->owner();
		
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
			
			cache.push_back( FSNode( index, gear::inscribe_unsigned_decimal( index ) ) );
		}
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_appls_QUAD_latest( const FSTreePtr&     parent,
	                                                              const plus::string&  name,
	                                                              const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &latest_appl_link_methods );
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_appls_QUAD_list( const FSTreePtr&     parent,
	                                                            const plus::string&  name,
	                                                            const void*          args )
	{
		return new_basic_directory( parent, name, appl_QUAD_list_lookup, appl_QUAD_list_iterate );
	}
	
	extern const premapped::mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[];
	
	const premapped::mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[] =
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
	
	static FSTreePtr appl_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return Premapped_Factory( parent, name, sys_mac_vol_list_N_dt_appls_QUAD_Mappings );
	}
	
	
	static plus::string generate_dt_icons_QUAD_QUAD_X( const FSTree* parent, const plus::string& name )
	{
		const FSTreePtr&   gparent = parent ->ParentRef();
		const FSTreePtr& gggparent = gparent->ParentRef()->ParentRef();
		
		const short selector = gear::parse_unsigned_decimal( name.c_str() );
		
		const ::OSType type    = parse_utf8_quad_name( parent ->Name() );
		const ::OSType creator = parse_utf8_quad_name( gparent->Name() );
		
		const short vRefNum = -gear::parse_unsigned_decimal( gggparent->Name().c_str() );
		
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
	
	
	static FSTreePtr icon_QUAD_QUAD_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !canonical_positive_integer::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return New_FSTree_Generated( parent, name, generate_dt_icons_QUAD_QUAD_X );
	}
	
	static void icon_QUAD_QUAD_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		const FSTreePtr&   gparent = parent ->ParentRef();
		const FSTreePtr& gggparent = gparent->ParentRef()->ParentRef();
		
		const ::OSType type    = parse_utf8_quad_name( parent ->Name() );
		const ::OSType creator = parse_utf8_quad_name( gparent->Name() );
		
		const short vRefNum = -gear::parse_unsigned_decimal( gggparent->Name().c_str() );
		
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
			
			cache.push_back( FSNode( selector, gear::inscribe_unsigned_decimal( selector ) ) );
		}
	}
	
	static FSTreePtr icon_QUAD_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return new_basic_directory( parent, name, icon_QUAD_QUAD_lookup, icon_QUAD_QUAD_iterate );
	}
	
	static FSTreePtr icon_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		validate_quad_name( name );
		
		return new_basic_directory( parent, name, icon_QUAD_lookup, NULL );
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_dir( const FSTreePtr&     parent,
	                                                const plus::string&  name,
	                                                const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &desktop_dir_methods );
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_appls( const FSTreePtr&     parent,
	                                                  const plus::string&  name,
	                                                  const void*          args )
	{
		return new_basic_directory( parent, name, appl_lookup, NULL );
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_icons( const FSTreePtr&     parent,
	                                                  const plus::string&  name,
	                                                  const void*          args )
	{
		return new_basic_directory( parent, name, icon_lookup, NULL );
	}
	
	
	const premapped::mapping sys_mac_vol_list_N_dt_Mappings[] =
	{
		{ "dir",    &new_sys_mac_vol_list_N_dt_dir   },
		{ "appls",  &new_sys_mac_vol_list_N_dt_appls },
		{ "icons",  &new_sys_mac_vol_list_N_dt_icons },
		
		{ NULL, NULL }
		
	};
	
}

