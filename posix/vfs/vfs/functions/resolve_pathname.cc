/*
	resolve_pathname.cc
	-------------------
*/

#include "vfs/functions/resolve_pathname.hh"

// Standard C/C++
#include <cstring>

// Standard C++
#include <algorithm>

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/lookup.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static node_ptr resolve_path( const node& that, const char*& begin, const char* end )
	{
		ASSERT( begin < end );
		
		ASSERT( begin[0] != '/' );
		
		const char* slash = std::find( begin, end, '/' );
		
		plus::string name( begin, slash );
		
		begin = slash;
		
		return lookup( that, name );
	}
	
	node_ptr resolve_relative_path( const node&  root,
	                                const char*  begin,
	                                std::size_t  length,
	                                const node&  current )
	{
		if ( length == 0 )
		{
			return &current;
		}
		
		node_ptr result = &current;
		
		const char* end = begin + length;
		
		result = resolve_path( *result, begin, end );
		
		while ( begin < end )
		{
			ASSERT( *begin == '/' );
			
			resolve_links_in_place( root, result );
			
			if ( is_file( *result ) )
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
				result = resolve_path( *result, begin, end );
			}
		}
		
		return result;
	}
	
	
	node_ptr resolve_absolute_path( const node&  root,
	                                const char*  begin,
	                                std::size_t  length )
	{
		ASSERT( length != 0 );
		
		ASSERT( *begin == '/' );
		
		const char* end = begin + length;
		
		while ( *++begin == '/' ) continue;
		
		length = end - begin;
		
		return length == 0 ? &root
		                   : resolve_relative_path( root, begin, length, root );
	}
	
	node_ptr resolve_absolute_path( const node&          root,
	                                const plus::string&  path )
	{
		return resolve_absolute_path( root, path.c_str(), path.length() );
	}
	
	
	node_ptr resolve_pathname( const node&  root,
	                           const char*  begin,
	                           std::size_t  length,
	                           const node&  current )
	{
		if ( length == 0 )
		{
			return &current;
		}
		
		if ( const bool absolute = *begin == '/' )
		{
			return resolve_absolute_path( root, begin, length );
		}
		
		return resolve_relative_path( root, begin, length, current );
	}
	
	node_ptr resolve_pathname( const node&  root,
	                           const char*  pathname,
	                           const node&  current )
	{
		return resolve_pathname( root, pathname, std::strlen( pathname ), current );
	}
	
	node_ptr resolve_pathname( const node&          root,
	                           const plus::string&  pathname,
	                           const node&          current )
	{
		return resolve_pathname( root, pathname.data(), pathname.size(), current );
	}
	
}
