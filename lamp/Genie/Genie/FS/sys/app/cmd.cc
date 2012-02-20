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

// Genie
#include "Genie/ProcessList.hh"
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/SymbolicLink.hh"


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
			
			spawn_process( target );
			
			return true;
		}
		
		return false;
	}
	
	
	static void unused_cmd_slot_symlink( const FSTree*        node,
	                                     const plus::string&  target )
	{
		if ( target.c_str()[0] != '/' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const Ped::CommandCode code = Ped::CommandCode( parse_utf8_quad_name( node->Name() ) );
		
		Ped::CommandHandler handler = Ped::GetCommandHandler( code );
		
		if ( handler != NULL  &&  handler != &exec_cmd )
		{
			p7::throw_errno( EACCES );
		}
		
		plus::string& value = the_command_map[ code ];
		
		if ( !value.empty() )
		{
			p7::throw_errno( EEXIST );
		}
		
		if ( handler == NULL )
		{
			Ped::SetCommandHandler( code, &exec_cmd );
		}
		
		value = target;
	}
	
	static const link_method_set unused_cmd_slot_link_methods =
	{
		NULL,
		NULL,
		&unused_cmd_slot_symlink
	};
	
	static node_method_set unused_cmd_slot_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&unused_cmd_slot_link_methods
	};
	
	
	class cmd_symlink : public FSTree_SymbolicLink
	{
		public:
			cmd_symlink( const FSTreePtr&     parent,
			             const plus::string&  name,
			             const plus::string&  target )
			:
				FSTree_SymbolicLink( parent, name, target )
			{
			}
			
			void Delete() const;
	};
	
	void cmd_symlink::Delete() const
	{
		const Ped::CommandCode code = Ped::CommandCode( parse_utf8_quad_name( Name() ) );
		
		cmd_map::iterator it = the_command_map.find( code );
		
		if ( it == the_command_map.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		the_command_map.erase( it );
		
		Ped::SetCommandHandler( code, NULL );
	}
	
	static FSTreePtr cmd_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		cmd_map::const_iterator it = the_command_map.find( parse_utf8_quad_name( name ) );
		
		if ( it != the_command_map.end() )
		{
			const plus::string& target = it->second;
			
			return new cmd_symlink( parent, name, target );
		}
		
		return new FSTree( parent, name, 0, &unused_cmd_slot_methods );
	}
	
	static void cmd_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		typedef cmd_map::iterator iterator;
		
		const iterator end = the_command_map.end();
		
		for ( iterator next = the_command_map.begin();  next != end;  )
		{
			iterator it = next++;
			
			const ino_t inode = 0;
			
			plus::var_string name;
			
			name += make_utf8_quad_name( it->first );
			
			cache.push_back( FSNode( inode, name ) );
		}
	}
	
	
	FSTreePtr New_FSTree_sys_app_cmd( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new_basic_directory( parent, name, cmd_lookup, cmd_iterate );
	}
	
}

