/*
	Genie/FS/gui/port.cc
	--------------------
*/

#include "Genie/FS/gui/port.hh"

// Standard C++
#include <map>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix-kernel
#include "relix/api/root.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/gui/port/ADDR.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef std::map< plus::string, const FSTree* > map_of_ports;
	
	static map_of_ports the_ports;
	
	
	static FSTreePtr port_lookup( const FSTree* parent, const plus::string& name )
	{
		map_of_ports::const_iterator it;
		
		it = the_ports.find( name );
		
		if ( it == the_ports.end() )
		{
			FSTreePtr port = fixed_dir( parent, name, gui_port_ADDR_Mappings, &remove_port );
			
			the_ports[ name ] = port.get();
			
			return port;
		}
		
		return it->second;
	}
	
	static void port_iterate( const FSTree* parent, vfs::dir_contents& cache )
	{
		map_of_ports::const_iterator end = the_ports.end();
		
		for ( map_of_ports::const_iterator it = the_ports.begin();  it != end;  ++it )
		{
			const FSTree* port = it->second;
			
			ino_t inode = (ino_t) port;  // coerce pointer to integer
			
			vfs::dir_entry node( inode, port->name() );
			
			cache.push_back( node );
		}
	}
	
	void remove_port( const FSTree* port )
	{
		remove_window_and_views_from_port( port );
		
		the_ports.erase( port->name() );
	}
	
	static const FSTree* gui_port()
	{
		static vfs::node_ptr port = vfs::resolve_absolute_path( *relix::root(), STR_LEN( "/gui/port" ) );
		
		return port.get();
	}
	
	FSTreePtr new_port()
	{
		const FSTree* parent = gui_port();
		
		FSTreePtr port = fixed_dir( parent, "/", gui_port_ADDR_Mappings, &remove_port );
		
		the_ports[ port->name() ] = port.get();
		
		return port;
	}
	
	FSTreePtr new_gui_port( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          args )
	{
		return new_basic_directory( parent, name, port_lookup, port_iterate );
	}
	
}

