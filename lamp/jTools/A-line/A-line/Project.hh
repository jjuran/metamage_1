/*	==========
 *	Project.hh
 *	==========
 */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
#include <map>
#include <string>
#include <vector>

// Boost
#include <boost/weak_ptr.hpp>

// A-line
#include "A-line/TargetInfo.hh"
#include "A-line/Task.hh"
#include "CompileDriver/ProjectConfig.hh"


namespace tool
{
	
	const std::vector< std::string >& get_values( const ConfData& config, const std::string& key );
	
	
	class Project
	{
		private:
			// This project's name within the A-line system.
			std::string its_name;
			Platform its_platform;
			// The name of the output file, if different from the project name.
			std::string its_program_filename;
			// The project's main folder.
			std::string its_dir_pathname;
			
			const ConfData& its_config_data;
			
			// The include path to a header to precompile.
			std::string its_precompiled_header_source_path;
			// What kind of product this project creates.
			ProductType its_product_type;
			// The names of all projects used directly or indirectly by this one.
			std::vector< std::string > its_used_project_names;
			// Directories to search for headers and unenumerated source files.
			std::vector< std::string > its_search_dir_pathnames;
			// Creator code / signature for output files.
			std::string its_creator_code;
			// Source files to compile.
			std::vector< std::string > its_source_file_pathnames;
			
			// maps include paths to absolute pathnames
			mutable std::map< std::string, std::string > its_include_map;
			
			boost::weak_ptr< Task > its_precompile_task;
			boost::weak_ptr< Task > its_static_lib_task;
		
		public:
			Project( const std::string&  name,
			         Platform            platform,
			         const std::string&  project_dir,
			         const ConfData&     conf_data );
			
			Platform get_platform() const  { return its_platform; }
			
			const std::string& Name         () const  { return its_name;             }
			const std::string& ProgramName  () const  { return its_program_filename; }
			const std::string& ProjectFolder() const  { return its_dir_pathname;     }
			
			ProductType Product() const  { return its_product_type; }
			
			bool HasPrecompiledHeader() const  { return !its_precompiled_header_source_path.empty(); }
			
			const std::string& PrecompiledHeaderSource() const  { return its_precompiled_header_source_path; }
			
			const std::vector< std::string >& AllUsedProjects() const  { return its_used_project_names;    }
			
			const std::vector< std::string >& SearchDirs() const  { return its_search_dir_pathnames; }
			
			const std::vector< std::string >& ToolSourceFiles() const  { return get_values( its_config_data, "tools"      ); }
			const std::vector< std::string >& LibImports()      const  { return get_values( its_config_data, "imports"    ); }
			const std::vector< std::string >& Frameworks()      const  { return get_values( its_config_data, "frameworks" ); }
			const std::vector< std::string >& UsedRezFiles()    const  { return get_values( its_config_data, "rez"        ); }
			const std::vector< std::string >& UsedRsrcFiles()   const  { return get_values( its_config_data, "rsrc"       ); }
			
			const std::string& CreatorCode() const  { return its_creator_code; }
			
			const std::vector< std::string >& Sources() const  { return its_source_file_pathnames; }
			
			std::string FindInclude           ( const std::string& include_path ) const;
			std::string FindIncludeRecursively( const std::string& include_path ) const;
			
			std::string FindResourceFile( const std::string& filename ) const;
			
			const boost::weak_ptr< Task >& get_precompile_task() const  { return its_precompile_task; }
			const boost::weak_ptr< Task >& get_static_lib_task() const  { return its_static_lib_task; }
			
			void set_precompile_task( const TaskPtr& task )  { its_precompile_task = task; }
			void set_static_lib_task( const TaskPtr& task )  { its_static_lib_task = task; }
	};
	
}

#endif

