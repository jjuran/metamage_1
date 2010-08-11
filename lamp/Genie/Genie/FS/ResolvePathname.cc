/*	==================
 *	ResolvePathname.cc
 *	==================
 */

#include "Genie/FS/ResolvePathname.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
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
		if ( length == 0 )
		{
			return current;
		}
		
		FSTreePtr result = current;
		
		const char* end = begin + length;
		
		result = result->ResolvePath( begin, end );
		
		while ( begin < end )
		{
			ResolveLinks_InPlace( result );
			
			ASSERT( *begin == '/' );
			
			if ( result->IsFile() )
			{
				if ( ++begin == end  ||  *begin == '/' )
				{
					// "file/" or "file//..."
					p7::throw_errno( ENOTDIR );
				}
			}
			else
			{
				while ( ++begin < end  &&  *begin == '/' ) continue;
			}
			
			if ( begin < end )
			{
				result = result->ResolvePath( begin, end );
			}
		}
		
		return result;
	}
	
}

