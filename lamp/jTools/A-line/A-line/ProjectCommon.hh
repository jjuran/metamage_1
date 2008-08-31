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


namespace ALine
{
	
	class Project;
	
	Project& GetProject( const std::string& project_name, Platform platform );
	
	void AddIncludeDir( const std::string& projName );
	
	std::string RezLocation( const std::string& filename );
	
	std::string FindInclude( const std::string& includePath, Platform platform );
	
	void AddRezFile( const std::string& file );
	
	time_t RecursivelyLatestDate( const std::string& includePath, Platform platform );
	
	time_t RecursivelyLatestDate( const std::string&  includePath,
	                              const std::string&  pathname,
	                              Platform            platform );
	
}

