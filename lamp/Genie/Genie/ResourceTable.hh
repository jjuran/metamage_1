/*	================
 *	ResourceTable.hh
 *	================
 */

#ifndef GENIE_RESOURCETABLE_HH
#define GENIE_RESOURCETABLE_HH

// Nitrogen core
#include "Nitrogen/OnlyOnce.h"


namespace Genie
{
	
	template < class Resource >
	class ResourceTable
	{
		typedef Resource Value, Object;
		
		struct Reference
		{
			Resource* resource;
			std::size_t refCount;
			
			Reference() : resource( NULL ), refCount( 0 )  {}
		};
		
		typedef std::vector< Reference > Vector;
		
		private:
		
			static bool IsEmptySlot( const Reference& slot )
			{
				return slot.resource == NULL;
			}
			
			static Vector& Table()
			{
				static Vector gTable;
				
				return gTable;
			}
		
			static void Destroy( std::size_t offset )
			{
				if ( IsEmptySlot( Table().at( offset ) ) )
				{
					// Do nothing for now
					return;
				}
				
				delete Table()[ offset ].resource;
				
				Table()[ offset ] = Reference();
			}
		
		public:
			static Resource& Lookup( std::size_t offset )
			{
				return *Table().at( offset ).resource;
			}
			
			static std::size_t RefMod( std::size_t offset, int delta )
			{
				std::size_t result = Table().at( offset ).refCount += delta;
				
				if ( result == 0 )
				{
					Destroy( offset );
				}
				
				return result;
			}
			
			static int Read( std::size_t offset, char* data, std::size_t byteCount )
			{
				return Table().at( offset ).resource->Read( data, byteCount );
			}
			
			static int Write( std::size_t offset, const char* data, std::size_t byteCount )
			{
				return Table().at( offset ).resource->Write( data, byteCount );
			}
			
			static std::size_t Add( std::auto_ptr< Resource > resource )
			{
				typedef typename Vector::iterator iterator;
				
				iterator found = std::find_if( Table().begin(),
				                               Table().end(),
				                               std::ptr_fun( IsEmptySlot ) );
				
				if ( found == Table().end() )
				{
					Table().resize( Table().size() + 1 );  // May invalidate iterators
					found = Table().end() - 1;
				}
				
				found->resource = resource.release();
				found->refCount = 0;
				
				return found - Table().begin();
			}
	};
	
}

#endif

