/*	==================
 *	ResolvePathname.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHNAME_HH
#define GENIE_FILESYSTEM_RESOLVEPATHNAME_HH

// Standard C/C++
#include <cstring>

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
	
	inline FSTreePtr ResolveRelativePath( const plus::string&  path,
	                                      const FSTreePtr&     current )
	{
		return ResolveRelativePath( path.c_str(), path.length(), current );
	}
	
	
	FSTreePtr ResolveAbsolutePath( const char*  begin,
	                               std::size_t  length );
	
	inline FSTreePtr ResolveAbsolutePath( const plus::string& path )
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
	
	inline FSTreePtr ResolvePathname( const plus::string&  pathname,
	                                  const FSTreePtr&     current = null_FSTreePtr )
	{
		return ResolvePathname( pathname.data(), pathname.size(), current );
	}
	
}

#endif

