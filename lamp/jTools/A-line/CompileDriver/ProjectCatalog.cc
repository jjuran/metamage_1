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
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform targetPlatform )
	{
		ProjectCatalog::const_iterator itProject = gProjectCatalog.find( name );
		
		if ( itProject == gProjectCatalog.end() )
		{
			RecursivelyAddPendingSubprojects();
			
			itProject = gProjectCatalog.find( name );
			
			if ( itProject == gProjectCatalog.end() )
			{
				throw NoSuchProject( name );
			}
		}
		
		const ProjectConfigCandidates& candidates = itProject->second;
		
		const ProjectConfigCandidates::const_iterator itCandidate = std::find_if( candidates.begin(),
		                                                                          candidates.end(),
		                                                                          std::bind2nd( more::ptr_fun( ProjectPlatformIsCompatible ),
		                                                                                        targetPlatform ) );
		
		if ( itCandidate == candidates.end() )
		{
			// FIXME:  Indicate that there are projects with this name,
			// but that they're not compatible with the current target
			throw NoSuchProject( name );
		}
		
		return itCandidate->second;
	}
	
	static void ScanItemForProjects( const std::string&                                       item,
	                                 std::back_insert_iterator< std::vector< std::string > >  output )
	{
		if ( io::directory_exists( item ) )
		{
			ScanDirForProjects( item, output );
		}
	}
	
	void ScanDirForProjects( const std::string&                                       dir,
	                         std::back_insert_iterator< std::vector< std::string > >  output )
	{
		std::string conf = dir / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			*output++ = conf;
			
			return;
		}
		
		typedef io::directory_contents_traits< std::string >::container_type directory_container;
		
		std::string confd = dir / "A-line.confd";
		
		typedef directory_container::const_iterator Iter;
		
		if ( io::directory_exists( confd ) )
		{
			directory_container contents = io::directory_contents( ( confd ) );
			
			std::copy( contents.begin(),
			           contents.end(),
			           output );
			
			return;
		}
		
		directory_container contents = io::directory_contents( ( dir ) );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind2nd( more::ptr_fun( ScanItemForProjects ),
		                             output ) );
	}
	
}

