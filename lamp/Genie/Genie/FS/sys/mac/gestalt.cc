/*
	Genie/FS/sys/mac/gestalt.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/gestalt.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// plus
#include "plus/var_string.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	using MacScribe::make_utf8_quad_name;
	using MacScribe::parse_utf8_quad_name;
	using MacScribe::quad_t;
	
	
	static inline bool is_valid_Gestalt_Selector_name( const plus::string& name )
	{
		quad_t decoded;
		
		try
		{
			decoded = parse_utf8_quad_name( name );
		}
		catch ( ... )
		{
			return false;
		}
		
		return mac::sys::gestalt_defined( decoded );
	}
	
	static FSTreePtr gestalt_lookup( const FSTree* parent, const plus::string& name )
	{
		if ( !is_valid_Gestalt_Selector_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name );
	}
	
	class gestalt_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( quad_t selector ) const
			{
				const bool valid = mac::sys::gestalt_defined( selector );
				
				const ino_t inode = 0;
				
				plus::var_string name;
				
				if ( !valid )
				{
					name += '.';
				}
				
				name += make_utf8_quad_name( selector );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	static void gestalt_iterate( const FSTree* parent, vfs::dir_contents& cache )
	{
		gestalt_IteratorConverter converter;
		
		using mac::sys::gestalt;
		using mac::sys::gestalt_handle;
		
		if ( gestalt_handle handle = (gestalt_handle) gestalt( 'tabl' ) )
		{
			if ( *handle == NULL )
			{
				Mac::ThrowOSStatus( nilHandleErr );
			}
			
			for ( int i = 0;  ;  ++i )
			{
				const quad_t selector = handle[ 0 ][ i ].selector;
				
				if ( selector == 0x7fffffff )
				{
					break;
				}
				
				cache.push_back( converter( selector ) );
			}
		}
	}
	
	FSTreePtr New_FSTree_sys_mac_gestalt( const FSTree*        parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new_basic_directory( parent, name, gestalt_lookup, gestalt_iterate );
	}
	
}

