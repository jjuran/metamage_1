/*	================
 *	ProjectCommon.hh
 *	================
 */

#ifndef ALINE_PROJECTCOMMON_HH
#define ALINE_PROJECTCOMMON_HH

// A-line
#include "A-line/Platform.hh"


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

