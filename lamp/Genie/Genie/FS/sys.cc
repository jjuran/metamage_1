/*
	Genie/FS/sys.cc
	---------------
*/

#include "Genie/FS/sys.hh"

// Standard C
#include <string.h>

// mac-app-utils
#include "mac_app/commands.hh"

// gear
#include "gear/hexadecimal.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"

// vfs-relix
#include "vfs/node/types/builtin.hh"

// relix
#include "relix/syscall/registry.hh"

// Genie
#include "Genie/FS/sys/app.hh"
#include "Genie/FS/sys/cpu.hh"
#include "Genie/FS/sys/mac.hh"
#include "Genie/FS/sys/type.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef const relix::system_call* SystemCallPtr;
	
	
	static plus::string name_of_syscall( SystemCallPtr key )
	{
		if ( key->name == NULL )
		{
			plus::string name;
			
			char* p = name.reset( sizeof '.' + sizeof (unsigned) * 2 );  // 9
			
			p[0] = '.';
			
			gear::encode_32_bit_hex( (unsigned) key, &p[1] );
			
			return name;
		}
		
		return key->name;
	}
	
	
	static const relix::system_call* LookUpSystemCallByName( const char* name )
	{
		using namespace relix;
		
		system_call* it  = the_syscall_array;
		system_call* end = it + the_last_syscall;
		
		for ( ;  it < end;  ++it )
		{
			if ( it->name != NULL  &&  strcmp( name, it->name ) == 0 )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
	static vfs::node_ptr syscall_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( LookUpSystemCallByName( name.c_str() ) == NULL )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name );
	}
	
	static void syscall_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		for ( int i = 0;  i < relix::the_last_syscall;  ++i )
		{
			const ino_t inode = i;
			
			plus::string name = name_of_syscall( &relix::the_syscall_array[ i ] );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
		}
	}
	
	static vfs::node_ptr New_sys_kernel_syscall( const vfs::node*     parent,
	                                             const plus::string&  name,
	                                             const void*          args )
	{
		return vfs::new_basic_directory( parent, name, syscall_lookup, syscall_iterate );
	}
	
	
	static int true_main()
	{
		return 0;
	}
	
	static int false_main()
	{
		return 1;
	}
	
	static int beep_main()
	{
		Mac::SysBeep();
		
		return 0;
	}
	
	static int quit_main()
	{
		mac::app::quit();
		
		return 0;
	}
	
	#define EXEC( main )  &vfs::builtin_factory, (const void*) &main
	
	extern const vfs::fixed_mapping sys_kernel_bin_Mappings[];
	
	const vfs::fixed_mapping sys_kernel_bin_Mappings[] =
	{
		{ "true",  EXEC( true_main  ) },
		{ "false", EXEC( false_main ) },
		
		{ "beep",  EXEC( beep_main  ) },
		{ "quit",  EXEC( quit_main  ) },
		
		{ NULL, NULL }
	};
	
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	extern const vfs::fixed_mapping sys_kernel_Mappings[];
	
	const vfs::fixed_mapping sys_kernel_Mappings[] =
	{
		{ "bin",     PREMAPPED( sys_kernel_bin_Mappings ) },
		
		{ "syscall", &New_sys_kernel_syscall },
		
		{ NULL, NULL }
	};
	
	const vfs::fixed_mapping sys_Mappings[] =
	{
		{ "app",    PREMAPPED( sys_app_Mappings    ) },
		{ "cpu",    PREMAPPED( sys_cpu_Mappings    ) },
		{ "kernel", PREMAPPED( sys_kernel_Mappings ) },
		{ "mac",    PREMAPPED( sys_mac_Mappings    ) },
		{ "type",   PREMAPPED( sys_type_Mappings   ) },
		
		{ NULL, NULL }
	};
	
}
