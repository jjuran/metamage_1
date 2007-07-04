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
	
	namespace P7 = POSeven;
	
	
	class PathnameComponentIterator
	{
		private:
			const char* pathname_begin;
			const char* pathname_end;
			const char* p;
			const char* q;
		
		public:
			PathnameComponentIterator( const std::string& pathname ) : pathname_begin( pathname.c_str() ),
			                                                           pathname_end  ( pathname_begin + pathname.size() ),
			                                                           p             ( pathname_begin ),
			                                                           q             ( p )
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
				P7::ThrowErrno( ELOOP );
			}
		}
		
		return link_count > 0;
	}
	
	FSTreePtr ResolvePathname( const std::string& pathname, const FSTreePtr& current )
	{
		PathnameComponentIterator path( pathname );
		
		FSTreePtr result = current;
		
		if ( path.Get().empty() )
		{
			result = FSRoot();
			
			path.Advance();
		}
		
		while ( !path.Done() )
		{
			ResolveLinks_InPlace( result );
			
			result = result->Lookup( path.Get() );
			
			path.Advance();
		}
		
		return result;
	}
	
}

