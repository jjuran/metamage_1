/*	============
 *	pathnames.hh
 *	============
 */

#pragma once

// Nitrogen
#include "Nitrogen/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	FSSpec ResolveUnixPathname( const char*          path,
	                            const N::FSDirSpec&  cwd = N::FSDirSpec() );

	inline FSSpec ResolveUnixPathname( const std::string&   pathname,
	                                   const N::FSDirSpec&  cwd = N::FSDirSpec() )
	{
		return ResolveUnixPathname( pathname.c_str(), cwd );
	}
	
}

