/*	==================
 *	ResolvePathname.cc
 *	==================
 */

#include "Genie/FileSystem/ResolvePathname.hh"

// Standard C/C++
#include <cstring>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class PathnameComponentIterator
	{
		private:
			const char* pathname_begin;
			const char* pathname_end;
			const char* p;
			const char* q;
		
		public:
			PathnameComponentIterator( const char* begin, const char* end )
			:
				pathname_begin( begin ),
				pathname_end  ( end   ),
				p             ( begin ),
				q             ( begin )
			{
				Scan();
			}
			
			void Scan()
			{
				q = std::strchr( p, '/' );
				
				if ( q == NULL )
				{
					q = pathname_end;
				}
			}
			
			bool Done() const
			{
				return p == pathname_end;
			}
			
			void Advance()
			{
				do
				{
					if ( q == pathname_end )
					{
						p = q;
						break;
					}
					
					p = q + 1;
					Scan();
				}
				while ( p == q );  // Skip empty segments
			}
			
			std::string Get() const
			{
				ASSERT( p != NULL );
				ASSERT( q != NULL );
				
				ASSERT( p <= q );
				
				ASSERT( p >= pathname_begin );
				ASSERT( q <= pathname_end   );
				
				return std::string( p, q );
			}
	};
	
	bool ResolveLink_InPlace( FSTreePtr& file )
	{
		if ( file->IsLink() )
		{
			file = file->ResolveLink();
			
			return true;
		}
		
		return false;
	}
	
	bool ResolveLinks_InPlace( FSTreePtr& file )
	{
		unsigned link_count = 0;
		
		while ( ResolveLink_InPlace( file ) )
		{
			++link_count;
			
			if ( link_count > 10 )
			{
				p7::throw_errno( ELOOP );
			}
		}
		
		return link_count > 0;
	}
	
	FSTreePtr ResolveRelativePath( const char*       begin,
	                               std::size_t       length,
	                               const FSTreePtr&  current )
	{
		FSTreePtr result = current;
		
		PathnameComponentIterator path( begin, begin + length );
		
		while ( !path.Done() )
		{
			ResolveLinks_InPlace( result );
			
			result = result->Lookup( path.Get() );
			
			path.Advance();
		}
		
		return result;
	}
	
}

