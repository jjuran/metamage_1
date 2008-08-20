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
#include "A-line/Project.hh"


namespace ALine
{
	
	Project& GetProject( const ProjName& projName );
	
	void AddIncludeDir( const ProjName& projName );
	
	std::string RezLocation( const FileName& filename );
	
	std::string FindInclude( const IncludePath& includePath );
	
	void AddRezFile( const std::string& file );
	
	time_t RecursivelyLatestDate( const IncludePath& includePath );
	time_t RecursivelyLatestDate( const IncludePath& includePath, const std::string& pathname );
	
}

