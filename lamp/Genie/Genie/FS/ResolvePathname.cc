/*	==================
 *	ResolvePathname.cc
 *	==================
 */

#include "Genie/FS/ResolvePathname.hh"

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/primitives/lookup.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/resolve.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool ResolveLink_InPlace( FSTreePtr& file )
	{
		if ( is_symlink( file ) )
		{
			file = resolve( file.get() );
			
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
	
	static FSTreePtr ResolvePath( const FSTree* node, const char*& begin, const char* end )
	{
		ASSERT( begin < end );
		
		ASSERT( begin[0] != '/' );
		
		const char* slash = std::find( begin, end, '/' );
		
		plus::string name( begin, slash );
		
		begin = slash;
		
		return lookup( node, name );
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
		
		result = ResolvePath( result.get(), begin, end );
		
		while ( begin < end )
		{
			ResolveLinks_InPlace( result );
			
			ASSERT( *begin == '/' );
			
			if ( is_file( result ) )
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
				result = ResolvePath( result.get(), begin, end );
			}
		}
		
		return result;
	}
	
	FSTreePtr ResolveAbsolutePath( const char*  begin,
	                               std::size_t  length )
	{
		ASSERT( length != 0 );
		
		ASSERT( *begin == '/' );
		
		const char* end = begin + length;
		
		while ( *++begin == '/' ) continue;
		
		length = end - begin;
		
		return length == 0 ? FSRoot()
		                   : ResolveRelativePath( begin, length, FSRoot() );
	}
	
}

