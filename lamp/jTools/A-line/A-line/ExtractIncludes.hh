/*	==================
 *	ExtractIncludes.hh
 *	==================
 */

#ifndef ALINE_EXTRACTINCLUDES_HH
#define ALINE_EXTRACTINCLUDES_HH

#if PRAGMA_ONCE
#pragma once
#endif

// A-line
#include "A-line/Includes.hh"


namespace tool
{
	
	IncludesCache ExtractIncludes( const std::string& pathname );
	
}

#endif

