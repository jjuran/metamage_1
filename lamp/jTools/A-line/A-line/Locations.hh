/*	============
 *	Locations.hh
 *	============
 */

#ifndef A_LINE_LOCATIONS_HH
#define A_LINE_LOCATIONS_HH

// plus
#include "plus/string.hh"


namespace tool
{
	
	const plus::string& mkdir_path( const plus::string& path );
	
	plus::string home_dir_pathname();
	
	plus::string UserSrcTreePath();
	
	plus::string get_user_cache_pathname();
	
	plus::string InstallDirPath();
	
	plus::string SourceDotListFile( const plus::string& projectPath );
	
	plus::string TargetDirPath( const plus::string& target );
	
	plus::string get_includes_union_pathname();
	
	plus::string LibrariesDirPath();
	plus::string RezzedDirPath();
	
	plus::string get_project_dependencies_pathname( const plus::string& project_name );
	
	plus::string ProjectDiagnosticsDirPath( const plus::string& proj );
	plus::string ProjectPrecompiledDirPath( const plus::string& proj );
	plus::string ProjectObjectsDirPath    ( const plus::string& proj );
	plus::string ProjectMetadataDirPath   ( const plus::string& proj );
	plus::string ProjectLinkedDirPath     ( const plus::string& proj );
	plus::string ProjectOutputDirPath     ( const plus::string& proj );
	
}

#endif
