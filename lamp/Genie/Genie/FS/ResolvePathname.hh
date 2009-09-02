/*	==================
 *	ResolvePathname.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHNAME_HH
#define GENIE_FILESYSTEM_RESOLVEPATHNAME_HH

// Standard C++
#include <string>

// Nucleus
#include "Nucleus/NAssert.h"

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
	
	
	inline FSTreePtr ResolvePathname( const std::string&  pathname,
	                                  const FSTreePtr&    current = FSTreePtr() )
	{
		const char* begin  = pathname.c_str();
		std::size_t length = pathname.length();
		
		if ( const bool absolute = *begin == '/' )
		{
			return ResolveAbsolutePath( begin, length );
		}
		
		return ResolveRelativePath( begin, length, current );
	}
	
}

#endif

