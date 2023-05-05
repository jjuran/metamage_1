/*	==========
 *	Project.hh
 *	==========
 */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/weak_ptr.hpp>

// plus
#include "plus/string.hh"

// vxo-string
#include "vxo-string/strmap.hh"

// A-line
#include "A-line/ProjectConfig.hh"
#include "A-line/TargetInfo.hh"
#include "A-line/Task.hh"


namespace tool
{
	
	const StringVector& get_values( const ConfData& config, const plus::string& key );
	
	
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
			StringVector its_used_project_names;
			// Directories to search for headers and unenumerated source files.
			StringVector its_search_dir_pathnames;
			// Directories to which search for source files is restricted.
			StringVector its_source_dir_pathnames;
			// Creator code / signature for output files.
			plus::string its_creator_code;
			// Source filenames which are tools.
			StringVector its_tool_filenames;
			// Number of source files which are tools (some names may be absent)
			std::size_t its_tool_count;
			// Source files to compile.
			StringVector its_source_file_pathnames;  // absolute
			
			// maps include paths to absolute pathnames
			mutable vxo::StrMap_to< vxo::String > its_include_map;
			
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
			
			const StringVector& AllUsedProjects() const  { return its_used_project_names;    }
			
			const StringVector& SearchDirs() const  { return its_search_dir_pathnames; }
			const StringVector& SourceDirs() const  { return its_source_dir_pathnames; }
			
			const StringVector& ToolSourceFiles() const  { return its_tool_filenames; }
			
			std::size_t ToolCount() const  { return its_tool_count; }
			
			const StringVector& LibImports()      const  { return get_values( its_config_data, "imports"    ); }
			const StringVector& Frameworks()      const  { return get_values( its_config_data, "frameworks" ); }
			const StringVector& UsedRezFiles()    const  { return get_values( its_config_data, "rez"        ); }
			
			const plus::string& CreatorCode() const  { return its_creator_code; }
			
			const StringVector& Sources() const  { return its_source_file_pathnames; }
			
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
