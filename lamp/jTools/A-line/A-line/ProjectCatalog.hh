/*	=================
 *	ProjectCatalog.hh
 *	=================
 */

#ifndef ALINE_PROJECTCATALOG_HH
#define ALINE_PROJECTCATALOG_HH

// plus
#include "plus/string.hh"

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif

// A-line
#include "A-line/Platform.hh"
#include "A-line/ProjectConfig.hh"
#include "A-line/StringVector.hh"


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
	
	void ScanDirForProjects( const plus::string&  dirPath,
	                         StringVector&        configs,
	                         StringVector&        folders );
	
	void write_catalog_cache( poseven::fd_t output   );
	void read_catalog_cache ( poseven::fd_t input_fd );
	
}

#endif
