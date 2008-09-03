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

// A-line
#include "A-line/Project.hh"


namespace CompileDriver
{
	
	using namespace io::path_descent_operators;
	
	
	// A map from platform requirements to project config data
	typedef std::map< PlatformDemands, ProjectConfig > ProjectConfigCandidates;
	
	// A map from project name to config file set
	typedef std::map< std::string, ProjectConfigCandidates > ProjectCatalog;
	
	
	static ProjectCatalog gProjectCatalog;
	
	
	void RecursivelyAddPendingSubprojects();
	
	
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
	
	static ProjectConfigCandidates& find_project_config_candidates( const std::string& project_name )
	{
		ProjectCatalog::iterator it = gProjectCatalog.find( project_name );
		
		if ( it == gProjectCatalog.end() )
		{
			RecursivelyAddPendingSubprojects();
			
			it = gProjectCatalog.find( project_name );
			
			if ( it == gProjectCatalog.end() )
			{
				throw NoSuchProject( project_name );
			}
		}
		
		return it->second;
	}
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform targetPlatform )
	{
		ProjectConfigCandidates& candidates = find_project_config_candidates( name );
		
		ProjectConfigCandidates::iterator it = std::find_if( candidates.begin(),
		                                                     candidates.end(),
		                                                     std::bind2nd( more::ptr_fun( ProjectPlatformIsCompatible ),
		                                                                   targetPlatform ) );
		
		if ( it == candidates.end() )
		{
			// FIXME:  Indicate that there are projects with this name,
			// but that they're not compatible with the current target
			throw NoSuchProject( name );
		}
		
		ProjectConfig& result = it->second;
		
		if ( result.get_refined_data() == NULL )
		{
			boost::shared_ptr< Project > new_project( new Project( name,
			                                                       targetPlatform,
			                                                       result.get_project_dir(),
			                                                       result.get_config_data() ) );
			
			result.set_refined_data( new_project );
			
			new_project->Study();
		}
		
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

