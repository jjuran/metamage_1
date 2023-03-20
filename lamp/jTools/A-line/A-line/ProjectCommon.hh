/*	================
 *	ProjectCommon.hh
 *	================
 */

#ifndef ALINE_PROJECTCOMMON_HH
#define ALINE_PROJECTCOMMON_HH

// plus
#include "plus/string_fwd.hh"

// A-line
#include "A-line/Platform.hh"


namespace tool
{
	
	class Project;
	
	Project& GetProject( const plus::string&  project_name,
	                     Platform             platform,
	                     bool                 optional = false );
	
}

#endif
