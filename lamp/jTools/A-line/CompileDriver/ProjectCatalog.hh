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

// poseven
#include "poseven/types/fd_t.hh"

// A-line
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectConfig.hh"


namespace tool
{
	
	struct ProjectConfig
	{
		private:
			std::string  its_pathname;
			std::string  its_project_dir;  // pathname
			ConfData     its_config_data;
		
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
			
			void set_pathname( const std::string& pathname )  { its_pathname = pathname; }
			
			void load_config();
	};
	
	
	void AddProjectConfigFile( const std::string&      name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config );
	
	const ProjectConfig& GetProjectConfig( const std::string& name, Platform platform );
	
	void ScanDirForProjects( const std::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< std::string > >  configs,
	                         std::back_insert_iterator< std::vector< std::string > >  folders );
	
	void write_catalog_cache( poseven::fd_t output   );
	void read_catalog_cache ( poseven::fd_t input_fd );
	
}

#endif

