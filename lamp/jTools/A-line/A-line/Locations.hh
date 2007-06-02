/*	============
 *	Locations.hh
 *	============
 */

#ifndef A_LINE_LOCATIONS_HH
#define A_LINE_LOCATIONS_HH

// Nitrogen
#include "Nitrogen/Files.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	N::FSDirSpec UserProjectsFolder();
	
	std::string UserProjectsPath();
	
	N::FSDirSpec ProjectSourcesFolder ( const N::FSDirSpec& folder );
	N::FSDirSpec ProjectIncludesFolder( const N::FSDirSpec& folder );
	
	std::string ProjectSourcesPath ( const std::string& projectPath );
	std::string ProjectIncludesPath( const std::string& projectPath );
	
	std::string SourceDotListFile( const std::string& projectPath );
	
	N::FSDirSpec ProjectObjectsFolder    ( const std::string& proj, const std::string& target );
	N::FSDirSpec ProjectLibrariesFolder  ( const std::string& proj, const std::string& target );
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj, const std::string& target );
	std::string ProjectPrecompiledDirPath( const std::string& proj, const std::string& target );
	std::string ProjectObjectsDirPath    ( const std::string& proj, const std::string& target );
	std::string ProjectLibrariesDirPath  ( const std::string& proj, const std::string& target );
	
}

#endif

