/*
	dynamic_group.cc
	----------------
*/

#include "vfs/node/types/dynamic_group.hh"

// POSIX
#include <sys/stat.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"


namespace vfs
{
	
	filehandle_ptr get_dynamic_element_from_node( const node*             that,
	                                              dynamic_element_getter  getter )
	{
		const unsigned id = gear::parse_unsigned_decimal( that->name().c_str() );
		
		return getter( id );
	}
	
	
	static node_ptr dynamic_group_lookup( const node*          that,
	                                      const plus::string&  name,
	                                      const node*          parent )
	{
		dynamic_group_extra& extra = *(dynamic_group_extra*) that->extra();
		
		const unsigned id = gear::parse_unsigned_decimal( name.c_str() );
		
		const dynamic_group& sequence = *extra.group;
		
		if ( sequence.find( id ) == sequence.end() )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return new node( parent,
		                 name,
		                 S_IFCHR | 0600,
		                 extra.methods );
	}
	
	static void dynamic_group_listdir( const node*    that,
	                                   dir_contents&  cache )
	{
		dynamic_group_extra& extra = *(dynamic_group_extra*) that->extra();
		
		const dynamic_group& sequence = *extra.group;
		
		typedef dynamic_group::const_iterator Iter;
		
		const Iter end = sequence.end();
		
		for ( Iter it = sequence.begin();  it != end;  ++it )
		{
			const int id = it->first;
			
			const ino_t inode = id;
			
			const plus::string name = gear::inscribe_unsigned_decimal( id );
			
			cache.push_back( dir_entry( inode, name ) );
		}
	}
	
	static const dir_method_set dynamic_group_dir_methods =
	{
		&dynamic_group_lookup,
		&dynamic_group_listdir
	};
	
	static const node_method_set dynamic_group_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&dynamic_group_dir_methods
	};
	
	node_ptr dynamic_group_factory( const node*          parent,
	                                const plus::string&  name,
	                                const void*          args )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFDIR | 0700,
		                         &dynamic_group_methods,
		                         sizeof (dynamic_group_extra) );
		
		dynamic_group_extra& extra = *(dynamic_group_extra*) result->extra();
		
		extra = *(dynamic_group_extra*) args;
		
		return result;
	}
	
}

