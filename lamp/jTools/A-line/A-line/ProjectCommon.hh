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


namespace ALine
{
	
	class Project;
	
	Project& GetProject( const std::string& projName );
	
	void AddIncludeDir( const std::string& projName );
	
	std::string RezLocation( const std::string& filename );
	
	std::string FindInclude( const std::string& includePath );
	
	void AddRezFile( const std::string& file );
	
	time_t RecursivelyLatestDate( const std::string& includePath );
	time_t RecursivelyLatestDate( const std::string& includePath, const std::string& pathname );
	
}

