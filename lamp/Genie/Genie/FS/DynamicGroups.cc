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

