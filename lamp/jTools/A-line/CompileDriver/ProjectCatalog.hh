/*	=================
 *	ProjectCatalog.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTCATALOG_HH
#define COMPILEDRIVER_PROJECTCATALOG_HH

// Standard C++
#include <iterator>
#include <map>
#include <string>
#include <vector>

// Boost
#include <boost/shared_ptr.hpp>

// A-line
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectConfig.hh"


namespace ALine
{
	
	class Project;
	
}

namespace CompileDriver
{
	
	using ALine::Project;
	
	
	struct ProjectConfig
	{
		private:
			std::string  its_pathname;
			std::string  its_project_dir;  // pathname
			ConfData     its_config_data;
			
			boost::shared_ptr< Project > its_refined_data;
		
		public:
			ProjectConfig()
			{
			}
			
			ProjectConfig( const std::string&  pathname,
			               const std::string&  project_dir,
			               const ConfData&     data ) : its_pathname   ( pathname    ),
			                                            its_project_dir( project_dir ),
			                                            its_config_data( data        )
			{
			}
			
			const std::string& get_pathname() const  { return its_pathname; }
			
			const std::string& get_project_dir() const  { return its_project_dir; }
			
			const ConfData& get_config_data() const  { return its_config_data; }
			
			const boost::shared_ptr< Project >& get_refined_data() const  { return its_refined_data; }
			
			void set_refined_data( const boost::shared_ptr< Project >& project )  { its_refined_data = project; }
	};
	
	// A map from platform requirements to project config data
	typedef std::map< PlatformDemands, ProjectConfig > ProjectConfigCandidates;
	
	// A map from project name to config file set
	typedef std::map< std::string, ProjectConfigCandidates > ProjectCatalog;
	
	
	void AddProjectConfigFile( const std::string&      name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config );
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform platform );
	
	void ScanDirForProjects( const std::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< std::string > >  configs,
	                         std::back_insert_iterator< std::vector< std::string > >  folders );
	
}

#endif

