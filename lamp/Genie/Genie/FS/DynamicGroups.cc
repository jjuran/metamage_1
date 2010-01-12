/*	================
 *	DynamicGroups.cc
 *	================
 */

#include "Genie/FS/DynamicGroups.hh"

// iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle > FSTree_Dynamic_N::Open( OpenFlags flags ) const
	{
		const unsigned id = std::atoi( Name().c_str() );
		
		return itsGetter( id );
	}
	
	class DynamicGroup_IteratorConverter
	{
		public:
			FSNode operator()( const DynamicGroup::value_type& value ) const
			{
				const unsigned id = value.first;
				
				const ino_t inode = id;
				
				const std::string name = iota::inscribe_unsigned_decimal( id );
				
				return FSNode( inode, name );
			}
	};
	
	FSTreePtr FSTree_DynamicGroup_Base::Lookup_Child( const std::string& name ) const
	{
		const unsigned id = atoi( name.c_str() );
		
		const DynamicGroup& sequence = ItemSequence();
		
		if ( sequence.find( id ) == sequence.end() )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return seize_ptr( new FSTree_Dynamic_N( Self(), name, Getter() ) );
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

