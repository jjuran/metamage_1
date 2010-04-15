/*	==================
 *	ResolvePathname.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHNAME_HH
#define GENIE_FILESYSTEM_RESOLVEPATHNAME_HH

// Standard C++
#include <string>

// Standard C/C++
#include <cstring>

// Debug
#include "debug/assert.hh"

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	bool ResolveLink_InPlace ( FSTreePtr& link );
	bool ResolveLinks_InPlace( FSTreePtr& link );
	
	FSTreePtr ResolveRelativePath( const char*       begin,
	                               std::size_t       length,
	                               const FSTreePtr&  current );
	
	inline FSTreePtr ResolveRelativePath( const std::string&  path,
	                                      const FSTreePtr&    current )
	{
		return ResolveRelativePath( path.c_str(), path.length(), current );
	}
	
	
	inline FSTreePtr ResolveAbsolutePath( const char*  begin,
	                                      std::size_t  length )
	{
		ASSERT( length != 0 );
		
		ASSERT( *begin == '/' );
		
		return ResolveRelativePath( begin + 1, length - 1, FSRoot() );
	}
	
	inline FSTreePtr ResolveAbsolutePath( const std::string& path )
	{
		return ResolveAbsolutePath( path.c_str(), path.length() );
	}
	
	
	inline FSTreePtr ResolvePathname( const char*       begin,
	                                  std::size_t       length,
	                                  const FSTreePtr&  current = null_FSTreePtr )
	{
		if ( const bool absolute = *begin == '/' )
		{
			return ResolveAbsolutePath( begin, length );
		}
		
		return ResolveRelativePath( begin, length, current );
	}
	
	inline FSTreePtr ResolvePathname( const char*       pathname,
	                                  const FSTreePtr&  current = null_FSTreePtr )
	{
		return ResolvePathname( pathname, std::strlen( pathname ), current );
	}
	
	inline FSTreePtr ResolvePathname( const std::string&  pathname,
	                                  const FSTreePtr&    current = null_FSTreePtr )
	{
		return ResolvePathname( pathname.data(), pathname.size(), current );
	}
	
	inline FSTreePtr ResolvePathname( const plus::string&  pathname,
	                                  const FSTreePtr&     current = null_FSTreePtr )
	{
		return ResolvePathname( pathname.data(), pathname.size(), current );
	}
	
}

#endif

