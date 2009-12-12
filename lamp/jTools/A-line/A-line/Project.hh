/*	==========
 *	Project.hh
 *	==========
 */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
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
			// The project's main folder.
			std::string its_dir_pathname;
			
			const ConfData& its_config_data;
			
			// What kind of product this project creates.
			ProductType its_product_type;
			// The name of the output file, if different from the project name.
			const std::string& its_program_filename;
			// The include path to a header to precompile.
			const std::string& its_prefix_source_path;
			// The names of all projects used directly or indirectly by this one.
			std::vector< std::string > its_used_project_names;
			// Directories to search for headers and unenumerated source files.
			std::vector< std::string > its_search_dir_pathnames;
			// Directories to which search for source files is restricted.
			std::vector< std::string > its_source_dir_pathnames;
			// Creator code / signature for output files.
			std::string its_creator_code;
			// Source filenames which are tools.
			std::vector< std::string > its_tool_filenames;
			// Number of source files which are tools (some names may be absent)
			std::size_t its_tool_count;
			// Source files to compile.
			std::vector< std::string > its_source_file_pathnames;  // absolute
			
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
			
			bool HasPrecompiledHeader() const  { return !its_prefix_source_path.empty(); }
			
			const std::string& PrecompiledHeaderSource() const  { return its_prefix_source_path; }
			
			const std::vector< std::string >& AllUsedProjects() const  { return its_used_project_names;    }
			
			const std::vector< std::string >& SearchDirs() const  { return its_search_dir_pathnames; }
			const std::vector< std::string >& SourceDirs() const  { return its_source_dir_pathnames; }
			
			const std::vector< std::string >& ToolSourceFiles() const  { return its_tool_filenames; }
			
			std::size_t ToolCount() const  { return its_tool_count; }
			
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

