/*
	Genie/FS/sys/app/cmd.cc
	-----------------------
*/

#include "Genie/FS/sys/app/cmd.hh"

// Standard C++
#include <map>

// plus
#include "plus/var_string.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/Commands.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/symbolic_link.hh"

// Genie
#include "Genie/ProcessList.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	using MacScribe::make_utf8_quad_name;
	using MacScribe::parse_utf8_quad_name;
	using MacScribe::quad_t;
	
	
	typedef std::map< quad_t, plus::string > cmd_map;
	
	static cmd_map the_command_map;
	
	
	static bool exec_cmd( Ped::CommandCode code )
	{
		cmd_map::const_iterator it = the_command_map.find( code );
		
		if ( it != the_command_map.end() )
		{
			const plus::string& target = it->second;
			
			spawn_process( target.c_str() );
			
			return true;
		}
		
		return false;
	}
	
	
	static void unused_cmd_slot_symlink( const vfs::node*     node,
	                                     const plus::string&  target )
	{
		if ( target.c_str()[0] != '/' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const Ped::CommandCode code = Ped::CommandCode( parse_utf8_quad_name( node->name() ) );
		
		Ped::CommandHandler handler = Ped::GetCommandHandler( code );
		
		if ( handler != NULL  &&  handler != &exec_cmd )
		{
			p7::throw_errno( EACCES );
		}
		
		plus::string& value = the_command_map[ code ];
		
		if ( handler == NULL )
		{
			Ped::SetCommandHandler( code, &exec_cmd );
		}
		
		value = target;
	}
	
	static const vfs::link_method_set unused_cmd_slot_link_methods =
	{
		NULL,
		NULL,
		&unused_cmd_slot_symlink
	};
	
	static const vfs::node_method_set unused_cmd_slot_methods =
	{
		NULL,
		NULL,
		&unused_cmd_slot_link_methods
	};
	
	
	static void cmd_symlink_remove( const vfs::node* that )
	{
		const Ped::CommandCode code = Ped::CommandCode( parse_utf8_quad_name( that->name() ) );
		
		cmd_map::iterator it = the_command_map.find( code );
		
		if ( it == the_command_map.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		the_command_map.erase( it );
		
		Ped::SetCommandHandler( code, NULL );
	}
	
	static vfs::node_ptr cmd_lookup( const vfs::node* parent, const plus::string& name )
	{
		cmd_map::const_iterator it = the_command_map.find( parse_utf8_quad_name( name ) );
		
		if ( it != the_command_map.end() )
		{
			const plus::string& target = it->second;
			
			return vfs::new_symbolic_link( parent,
			                               name,
			                               target,
			                               &cmd_symlink_remove );
		}
		
		return new vfs::node( parent, name, 0, &unused_cmd_slot_methods );
	}
	
	static void cmd_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		typedef cmd_map::iterator iterator;
		
		const iterator end = the_command_map.end();
		
		for ( iterator next = the_command_map.begin();  next != end;  )
		{
			iterator it = next++;
			
			const ino_t inode = 0;
			
			plus::var_string name;
			
			name += make_utf8_quad_name( it->first );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
		}
	}
	
	
	vfs::node_ptr New_FSTree_sys_app_cmd( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return vfs::new_basic_directory( parent, name, cmd_lookup, cmd_iterate );
	}
	
}
