/*
	Genie/FS/gui/port.cc
	--------------------
*/

#include "Genie/FS/gui/port.hh"

// Standard C++
#include <set>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/hexidecimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/gui/port/ADDR.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef std::set< const FSTree* > set_of_ports;
	
	static set_of_ports the_ports;
	
	
	static FSTreePtr port_lookup( const FSTree* parent, const plus::string& name )
	{
		set_of_ports::const_iterator it;
		
		const bool canonical = canonical_32_bit_hex::applies( name );
		
		if ( canonical )
		{
			const FSTree* key = (const FSTree*) plus::decode_32_bit_hex( name );
			
			it = the_ports.find( key );
		}
		
		if ( !canonical  ||  it == the_ports.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return FSTreePtr( *it );
	}
	
	static void port_iterate( const FSTree* parent, vfs::dir_contents& cache )
	{
		set_of_ports::const_iterator end = the_ports.end();
		
		for ( set_of_ports::const_iterator it = the_ports.begin();  it != end;  ++it )
		{
			const FSTree* port = *it;
			
			ino_t inode = (ino_t) port;  // coerce pointer to integer
			
			vfs::dir_entry node( inode, port->name() );
			
			cache.push_back( node );
		}
	}
	
	void remove_port( const FSTree* port )
	{
		remove_window_and_views_from_port( port );
		
		the_ports.erase( port );
	}
	
	static const FSTree* gui_port()
	{
		static FSTreePtr node = ResolveAbsolutePath( STR_LEN( "/gui/port" ) );
		
		return node.get();
	}
	
	FSTreePtr new_port()
	{
		const FSTree* parent = gui_port();
		
		FSTreePtr port = fixed_dir( parent, "/", sys_port_ADDR_Mappings, &remove_port );
		
		the_ports.insert( port.get() );
		
		return port;
	}
	
	FSTreePtr New_sys_port( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          args )
	{
		return new_basic_directory( parent, name, port_lookup, port_iterate );
	}
	
}

