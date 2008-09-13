/*	================
 *	ProjectCommon.hh
 *	================
 */

#pragma once

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

