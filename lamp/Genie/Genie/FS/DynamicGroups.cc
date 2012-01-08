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
	
	FSTree_Dynamic_N::FSTree_Dynamic_N( const FSTreePtr&      parent,
	                                    const plus::string&   name,
	                                    DynamicElementGetter  getter )
	:
		FSTree( parent, name, S_IFCHR | 0600 ),
		itsGetter( getter )
	{
	}
	
	IOPtr FSTree_Dynamic_N::Open( OpenFlags flags ) const
	{
		const unsigned id = gear::parse_unsigned_decimal( Name().c_str() );
		
		return itsGetter( id );
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
		
		return new FSTree_Dynamic_N( (parent ? parent : this)->Self(), name, Getter() );
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

