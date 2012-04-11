/*	================
 *	DynamicGroups.cc
 *	================
 */

#include "Genie/FS/DynamicGroups.hh"

// Standard C++
#include <algorithm>

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

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"


namespace Genie
{
	
	IOPtr get_dynamic_element_from_node( const FSTree* node, DynamicElementGetter getter )
	{
		const unsigned id = gear::parse_unsigned_decimal( node->name().c_str() );
		
		return getter( id );
	}
	
	
	class DynamicGroup_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( const DynamicGroup::value_type& value ) const
			{
				const unsigned id = value.first;
				
				const ino_t inode = id;
				
				const plus::string name = gear::inscribe_unsigned_decimal( id );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	
	static FSTreePtr dynamic_group_lookup( const FSTree*        node,
	                                       const plus::string&  name,
	                                       const FSTree*        parent )
	{
		dynamic_group_extra& extra = *(dynamic_group_extra*) node->extra();
		
		const unsigned id = gear::parse_unsigned_decimal( name.c_str() );
		
		const DynamicGroup& sequence = *extra.group;
		
		if ( sequence.find( id ) == sequence.end() )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return new FSTree( parent,
		                   name,
		                   S_IFCHR | 0600,
		                   extra.methods );
	}
	
	static void dynamic_group_listdir( const FSTree*       node,
	                                   vfs::dir_contents&  cache )
	{
		dynamic_group_extra& extra = *(dynamic_group_extra*) node->extra();
		
		DynamicGroup_IteratorConverter converter;
		
		const DynamicGroup& sequence = *extra.group;
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
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
	
	FSTreePtr dynamic_group_factory( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args )
	{
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFDIR | 0700,
		                             &dynamic_group_methods,
		                             sizeof (dynamic_group_extra) );
		
		dynamic_group_extra& extra = *(dynamic_group_extra*) result->extra();
		
		extra = *(dynamic_group_extra*) args;
		
		return result;
	}
	
}

