/*	================
 *	ProjectCommon.hh
 *	================
 */

#ifndef ALINE_PROJECTCOMMON_HH
#define ALINE_PROJECTCOMMON_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>
#include <vector>

// Standard C
#include <time.h>

// A-line
#include "CompileDriver/Platform.hh"


namespace tool
{
	
	class Project;
	
	Project& GetProject( const std::string& project_name, Platform platform );
	
}

#endif

