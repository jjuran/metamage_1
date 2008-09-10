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
	
	const std::string& mkdir_path( const std::string& path );
	
	std::string UserSrcTreePath();
	
	std::string get_user_cache_pathname();
	
	std::string ProjectIncludesPath( const std::string& projectPath );
	
	std::string SourceDotListFile( const std::string& projectPath );
	
	std::string TargetDirPath( const std::string& target );
	
	std::string LibrariesDirPath();
	std::string RezzedDirPath();
	
	std::string get_project_dependencies_pathname( const std::string& project_name );
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj );
	std::string ProjectPrecompiledDirPath( const std::string& proj );
	std::string ProjectObjectsDirPath    ( const std::string& proj );
	std::string ProjectMetadataDirPath   ( const std::string& proj );
	std::string ProjectLinkedDirPath     ( const std::string& proj );
	std::string ProjectOutputDirPath     ( const std::string& proj );
	
}

#endif

