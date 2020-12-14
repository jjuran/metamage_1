/*	==========
 *	Project.hh
 *	==========
 */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
#include <vector>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/weak_ptr.hpp>

// plus
#include "plus/string.hh"

// A-line
#include "A-line/ProjectConfig.hh"
#include "A-line/TargetInfo.hh"
#include "A-line/Task.hh"


namespace tool
{
	
	const std::vector< plus::string >& get_values( const ConfData& config, const plus::string& key );
	
	
	class Project
	{
		private:
			// This project's name within the A-line system.
			plus::string its_name;
			Platform its_platform;
			// The project's main folder.
			plus::string its_dir_pathname;
			
			const ConfData& its_config_data;
			
			// What kind of product this project creates.
			ProductType its_product_type;
			// The name of the output file, if different from the project name.
			const plus::string& its_program_filename;
			// The include path to a header to precompile.
			const plus::string& its_prefix_source_path;
			// The names of all projects used directly or indirectly by this one.
			std::vector< plus::string > its_used_project_names;
			// Directories to search for headers and unenumerated source files.
			std::vector< plus::string > its_search_dir_pathnames;
			// Directories to which search for source files is restricted.
			std::vector< plus::string > its_source_dir_pathnames;
			// Creator code / signature for output files.
			plus::string its_creator_code;
			// Source filenames which are tools.
			std::vector< plus::string > its_tool_filenames;
			// Number of source files which are tools (some names may be absent)
			std::size_t its_tool_count;
			// Source files to compile.
			std::vector< plus::string > its_source_file_pathnames;  // absolute
			
			// maps include paths to absolute pathnames
			mutable std::map< plus::string, plus::string > its_include_map;
			
			boost::weak_ptr< Task > its_precompile_task;
			boost::weak_ptr< Task > its_static_lib_task;
		
		public:
			Project();
			
			Project( const plus::string&  name,
			         Platform             platform,
			         const plus::string&  project_dir,
			         const ConfData&      conf_data );
			
			Platform get_platform() const  { return its_platform; }
			
			const plus::string& Name         () const  { return its_name;             }
			const plus::string& ProgramName  () const  { return its_program_filename; }
			const plus::string& ProjectFolder() const  { return its_dir_pathname;     }
			
			ProductType Product() const  { return its_product_type; }
			
			bool HasPrecompiledHeader() const  { return !its_prefix_source_path.empty(); }
			
			const plus::string& PrecompiledHeaderSource() const  { return its_prefix_source_path; }
			
			const std::vector< plus::string >& AllUsedProjects() const  { return its_used_project_names;    }
			
			const std::vector< plus::string >& SearchDirs() const  { return its_search_dir_pathnames; }
			const std::vector< plus::string >& SourceDirs() const  { return its_source_dir_pathnames; }
			
			const std::vector< plus::string >& ToolSourceFiles() const  { return its_tool_filenames; }
			
			std::size_t ToolCount() const  { return its_tool_count; }
			
			const std::vector< plus::string >& LibImports()      const  { return get_values( its_config_data, "imports"    ); }
			const std::vector< plus::string >& Frameworks()      const  { return get_values( its_config_data, "frameworks" ); }
			const std::vector< plus::string >& UsedRezFiles()    const  { return get_values( its_config_data, "rez"        ); }
			
			const plus::string& CreatorCode() const  { return its_creator_code; }
			
			const std::vector< plus::string >& Sources() const  { return its_source_file_pathnames; }
			
			plus::string FindInclude           ( const plus::string& include_path ) const;
			
			const plus::string& FindIncludeRecursively( const plus::string& include_path ) const;
			
			plus::string FindResourceFile( const plus::string& filename ) const;
			
			const boost::weak_ptr< Task >& get_precompile_task() const  { return its_precompile_task; }
			const boost::weak_ptr< Task >& get_static_lib_task() const  { return its_static_lib_task; }
			
			void set_precompile_task( const TaskPtr& task )  { its_precompile_task = task; }
			void set_static_lib_task( const TaskPtr& task )  { its_static_lib_task = task; }
	};
	
}

#endif
