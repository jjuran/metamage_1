/*	=================
 *	ProjectCatalog.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTCATALOG_HH
#define COMPILEDRIVER_PROJECTCATALOG_HH

// Standard C++
#include <iterator>
#include <map>
#include <vector>

// plus
#include "plus/string.hh"

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
			plus::string  its_pathname;
			plus::string  its_project_dir;  // pathname
			ConfData      its_config_data;
		
		public:
			ProjectConfig()
			{
			}
			
			ProjectConfig( const plus::string&  pathname,
			               const plus::string&  project_dir,
			               const ConfData&      data )
			:
				its_pathname   ( pathname    ),
				its_project_dir( project_dir ),
				its_config_data( data        )
			{
			}
			
			const plus::string& get_pathname() const  { return its_pathname; }
			
			const plus::string& get_project_dir() const  { return its_project_dir; }
			
			const ConfData& get_config_data() const  { return its_config_data; }
			
			void set_pathname( const plus::string& pathname )  { its_pathname = pathname; }
			
			void load_config();
	};
	
	
	void AddProjectConfigFile( const plus::string&     name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config );
	
	const ProjectConfig& GetProjectConfig( const plus::string& name, Platform platform );
	
	void ScanDirForProjects( const plus::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< plus::string > >  configs,
	                         std::back_insert_iterator< std::vector< plus::string > >  folders );
	
	void write_catalog_cache( poseven::fd_t output   );
	void read_catalog_cache ( poseven::fd_t input_fd );
	
}

#endif

