/*	=================
 *	ProjectCatalog.cc
 *	=================
 */

#include "CompileDriver/ProjectCatalog.hh"

// Standard C++
#include <string>
#include <vector>

// Io
#include "io/files.hh"
#include "io/walk.hh"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// MoreFunctional
#include "PointerToFunction.hh"


namespace CompileDriver
{
	
	using namespace io::path_descent_operators;
	
	
	// A map from platform requirements to project config data
	typedef std::map< PlatformDemands, ProjectConfig > ProjectConfigCandidates;
	
	// A map from project name to config file set
	typedef std::map< std::string, ProjectConfigCandidates > ProjectCatalog;
	
	
	static ProjectCatalog gProjectCatalog;
	
	
	bool AddPendingSubprojects();
	
	
	void AddProjectConfigFile( const std::string&      name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config )
	{
		gProjectCatalog[ name ][ demands ] = config;
	}
	
	static bool ProjectPlatformIsCompatible( const ProjectConfigCandidates::value_type&  candidate,
	                                         Platform                                    target )
	{
		const PlatformDemands& projectDemands = candidate.first;
		
		return projectDemands.Test( target );
	}
	
	static const ProjectConfigCandidates& find_project_config_candidates( const std::string& project_name )
	{
		ProjectCatalog::const_iterator it;
		
		do
		{
			it = gProjectCatalog.find( project_name );
		}
		while ( it == gProjectCatalog.end()  &&  AddPendingSubprojects() );
		
		if ( it == gProjectCatalog.end() )
		{
			throw NoSuchProject( project_name );
		}
		
		return it->second;
	}
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform targetPlatform )
	{
		const ProjectConfigCandidates& candidates = find_project_config_candidates( name );
		
		ProjectConfigCandidates::const_iterator it;
		
		do
		{
			it = std::find_if( candidates.begin(),
			                   candidates.end(),
			                   std::bind2nd( more::ptr_fun( ProjectPlatformIsCompatible ),
			                                 targetPlatform ) );
		}
		while ( it == candidates.end()  &&  AddPendingSubprojects() );
		
		if ( it == candidates.end() )
		{
			// FIXME:  Indicate that there are projects with this name,
			// but that they're not compatible with the current target
			throw NoSuchProject( name );
		}
		
		const ProjectConfig& result = it->second;
		
		return result;
	}
	
	void ScanDirForProjects( const std::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< std::string > >  configs,
	                         std::back_insert_iterator< std::vector< std::string > >  folders )
	{
		if ( !io::directory_exists( dirPath ) )
		{
			return;
		}
		
		std::string conf = dirPath / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			*configs++ = conf;
			
			return;
		}
		
		typedef io::directory_contents_traits< std::string >::container_type directory_container;
		
		std::string confd = dirPath / "A-line.confd";
		
		bool has_confd = io::directory_exists( confd );
		
		directory_container contents = io::directory_contents( has_confd ? confd : dirPath );
		
		std::copy( contents.begin(),
		           contents.end(),
		           has_confd ? configs : folders );
	}
	
}

