/*	============
 *	Locations.hh
 *	============
 */

#ifndef A_LINE_LOCATIONS_HH
#define A_LINE_LOCATIONS_HH

// Standard C++
#include <string>


namespace ALine
{
	
	std::string UserProjectsPath();
	
	std::string ProjectSourcesPath ( const std::string& projectPath );
	std::string ProjectIncludesPath( const std::string& projectPath );
	
	std::string SourceDotListFile( const std::string& projectPath );
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj, const std::string& target );
	std::string ProjectPrecompiledDirPath( const std::string& proj, const std::string& target );
	std::string ProjectObjectsDirPath    ( const std::string& proj, const std::string& target );
	std::string ProjectLibrariesDirPath  ( const std::string& proj, const std::string& target );
	
}

#endif

