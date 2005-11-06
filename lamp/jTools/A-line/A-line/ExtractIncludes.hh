/*	==================
 *	ExtractIncludes.hh
 *	==================
 *	
 *	Implemented by ExtractIncludes.cc
 */

#pragma once

// C++
#include <string>
#include <vector>


struct FSSpec;

namespace ALine {
	
	extern std::vector< std::string > ExtractIncludes( const FSSpec& file );
	
}

