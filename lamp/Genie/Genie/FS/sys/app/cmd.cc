/*
	Genie/FS/sys/app/cmd.cc
	-----------------------
*/

#include "Genie/FS/sys/app/cmd.hh"

// Standard C++
#include <map>

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/Commands.hh"

// Genie
#include "Genie/ProcessList.hh"
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/quad_name.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/SymbolicLink.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
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
	
	
	class unused_cmd_slot : public FSTree
	{
		public:
			unused_cmd_slot( const FSTreePtr&     parent,
			                 const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			bool Exists() const  { return false; }
			
			void SymLink( const plus::string& target ) const;
	};
	
	void unused_cmd_slot::SymLink( const plus::string& target ) const
	{
		if ( target.c_str()[0] != '/' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const Ped::CommandCode code = Ped::CommandCode( parse_quad_name( Name() ) );
		
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
		const Ped::CommandCode code = Ped::CommandCode( parse_quad_name( Name() ) );
		
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
		cmd_map::const_iterator it = the_command_map.find( parse_quad_name( name ) );
		
		if ( it != the_command_map.end() )
		{
			const plus::string& target = it->second;
			
			return new cmd_symlink( parent, name, target );
		}
		
		return new unused_cmd_slot( parent, name );
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
			
			inscribe_quad_name( name, it->first );
			
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

