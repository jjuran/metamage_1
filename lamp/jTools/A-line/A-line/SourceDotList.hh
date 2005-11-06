/*	================
 *	SourceDotList.hh
 *	================
 *	
 *	Implemented by SourceDotList.cc
 */

#pragma once

// C++
#include <string>
#include <vector>


struct FSSpec;

namespace ALine
{
	
	std::vector< std::string > ReadSourceDotList( const FSSpec& sourceList );
	
}

