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

// Genie
#include "Genie/FS/FSTreeCache.hh"
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
			FSNode operator()( const DynamicGroup::value_type& value ) const
			{
				const unsigned id = value.first;
				
				const ino_t inode = id;
				
				const plus::string name = gear::inscribe_unsigned_decimal( id );
				
				return FSNode( inode, name );
			}
	};
	
	
	static FSTreePtr dynamic_group_lookup( const FSTree*        node,
	                                       const plus::string&  name,
	                                       const FSTree*        parent )
	{
		const FSTree_DynamicGroup_Base* file = static_cast< const FSTree_DynamicGroup_Base* >( node );
		
		return file->Lookup_Child( name, parent );
	}
	
	static void dynamic_group_listdir( const FSTree*  node,
	                                   FSTreeCache&   cache )
	{
		const FSTree_DynamicGroup_Base* file = static_cast< const FSTree_DynamicGroup_Base* >( node );
		
		file->IterateIntoCache( cache );
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
	
	FSTree_DynamicGroup_Base::FSTree_DynamicGroup_Base( const FSTreePtr&     parent,
	                                                    const plus::string&  name )
	:
		FSTree( parent, name, S_IFDIR | 0700, &dynamic_group_methods )
	{
	}
	
	FSTreePtr FSTree_DynamicGroup_Base::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		const unsigned id = gear::parse_unsigned_decimal( name.c_str() );
		
		const DynamicGroup& sequence = ItemSequence();
		
		if ( sequence.find( id ) == sequence.end() )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return new FSTree( parent,
		                   name,
		                   S_IFCHR | 0600,
		                   node_methods() );
	}
	
	void FSTree_DynamicGroup_Base::IterateIntoCache( FSTreeCache& cache ) const
	{
		DynamicGroup_IteratorConverter converter;
		
		const DynamicGroup& sequence = ItemSequence();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
}

