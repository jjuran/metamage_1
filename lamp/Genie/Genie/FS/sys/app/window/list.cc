/*
	Genie/FS/sys/app/window/list.cc
	-------------------------------
*/

#include "Genie/FS/sys/app/window/list.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// plus
#include "plus/hexidecimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/sys/app/window/list/REF.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"
#include "Genie/Utilities/WindowList_contains.hh"


namespace Genie
{
	
	static bool is_valid_WindowRef_name( const plus::string& name )
	{
		if ( !canonical_32_bit_hex::applies( name ) )
		{
			return false;
		}
		
		const WindowRef window = (WindowRef) plus::decode_32_bit_hex( name );
		
		return WindowList_contains( window );
	}
	
	static FSTreePtr WindowRef_lookup( const FSTree* parent, const plus::string& name )
	{
		if ( !is_valid_WindowRef_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_app_window_list_REF_Mappings );
	}
	
	static void WindowRef_iterate( const FSTree* parent, vfs::dir_contents& cache )
	{
		WindowRef window = FrontWindow();
		
		while ( window != NULL )
		{
			const ino_t inode = 0;
			
			plus::string name = plus::encode_32_bit_hex( (unsigned) window );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
			
			window = GetNextWindow( window );
		}
	}
	
	FSTreePtr New_FSTree_sys_app_window_list( const FSTree*        parent,
	                                          const plus::string&  name,
	                                          const void*          args )
	{
		return new_basic_directory( parent, name, WindowRef_lookup, WindowRef_iterate );
	}
	
}

