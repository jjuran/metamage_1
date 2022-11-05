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

// mac-file-utils
#include "mac_file/desktop.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/basic_directory.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	using MacScribe::invalid_quad_name;
	using MacScribe::parse_utf8_quad_name;
	
	
	static FSSpec DTGetAPPL( const vfs::node* appls_quad, short index = 0 )
	{
		using mac::file::get_desktop_APPL;
		
		const ::OSType creator = parse_utf8_quad_name( appls_quad->name() );
		
		const vfs::node* great_x2_grandparent = appls_quad->owner()->owner()->owner();
		
		const short vRefNum = -gear::parse_unsigned_decimal( great_x2_grandparent->name().c_str() );
		
		FSSpec file;
		
		Mac::ThrowOSStatus( get_desktop_APPL( file, vRefNum, creator, index ) );
		
		return file;
	}
	
	static vfs::node_ptr latest_appl_link_resolve( const vfs::node* that )
	{
		const vfs::node* parent = that->owner();
		
		const FSSpec file = DTGetAPPL( parent );
		
		return FSTreeFromFSSpec( file );
	}
	
	static const vfs::link_method_set latest_appl_link_link_methods =
	{
		NULL,
		&latest_appl_link_resolve
	};
	
	static const vfs::node_method_set latest_appl_link_methods =
	{
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
	
	static const vfs::link_method_set dt_appls_QUAD_list_N__link_methods =
	{
		NULL,
		&dt_appls_QUAD_list_N_resolve
	};
	
	static const vfs::node_method_set dt_appls_QUAD_list_N_methods =
	{
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
		try
		{
			const FSSpec file = DTGetAPPL( parent );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err == paramErr )
			{
				p7::throw_errno( ENOENT );
			}
		}
		
		return new vfs::node( parent, name, S_IFLNK | 0777, &latest_appl_link_methods );
	}
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_appls_QUAD_list( const vfs::node*     parent,
	                                                                const plus::string&  name,
	                                                                const void*          args )
	{
		return vfs::new_basic_directory( parent, name, appl_QUAD_list_lookup, appl_QUAD_list_iterate );
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
	
	
	static vfs::node_ptr new_sys_mac_vol_list_N_dt_appls( const vfs::node*     parent,
	                                                      const plus::string&  name,
	                                                      const void*          args )
	{
		return vfs::new_basic_directory( parent, name, appl_lookup, NULL );
	}
	
	
	const vfs::fixed_mapping sys_mac_vol_list_N_dt_Mappings[] =
	{
		{ "appls",  &new_sys_mac_vol_list_N_dt_appls },
		
		{ NULL, NULL }
		
	};
	
}
