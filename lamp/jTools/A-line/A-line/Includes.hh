/*	===========
 *	Includes.hh
 *	===========
 */

#ifndef ALINE_INCLUDES_HH
#define ALINE_INCLUDES_HH

#if PRAGMA_ONCE
#pragma once
#endif

// C++
#include <string>
#include <vector>


namespace tool
{
	
	struct IncludesCache
	{
		std::vector< std::string > user;
		std::vector< std::string > system;
	};
	
	const IncludesCache& GetIncludes( const std::string& pathname );
	
}

#endif

