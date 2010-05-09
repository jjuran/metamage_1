/*	================
 *	ProjectCommon.hh
 *	================
 */

#ifndef ALINE_PROJECTCOMMON_HH
#define ALINE_PROJECTCOMMON_HH

// Standard C++
#include <vector>

// Standard C
#include <time.h>

// A-line
#include "CompileDriver/Platform.hh"


namespace plus
{
	
	class string;
	
}

namespace tool
{
	
	class Project;
	
	Project& GetProject( const plus::string& project_name, Platform platform );
	
}

#endif

