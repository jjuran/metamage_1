/*	==========
 *	Project.hh
 *	==========
  */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
#include <string>
#include <vector>

// A-line
#include "A-line/TargetInfo.hh"


namespace ALine
{
	
	class Project
	{
		private:
			// This project's name within the A-line system.
			std::string its_name;
			// The name of the output file, if different from the project name.
			std::string its_program_filename;
			// The project's main folder.
			std::string its_dir_pathname;
			// The include path to a header to precompile.
			std::string its_precompiled_header_source_path;
			// What kind of product this project creates.
			ProductType its_product_type;
			// The names of all projects used directly or indirectly by this one.
			std::vector< std::string > its_used_project_names;
			// Directories to search for headers and unenumerated source files.
			std::vector< std::string > its_search_dir_pathnames;
			// Source files to compile, by pathname
			std::vector< std::string > its_source_paths;
			// Various things to link with.
			std::vector< std::string > its_rez_filenames, its_rsrc_filenames;
			std::vector< std::string > its_lib_import_specs;
			std::vector< std::string > its_framework_names;
			// Creator code / signature for output files.
			std::string its_creator_code;
			// Source files to compile.
			std::vector< std::string > its_source_file_pathnames;
			// Tool source files.
			std::vector< std::string > its_tool_source_filenames;
		
		public:
			Project( const std::string& name );
			
			std::string Name() const  { return its_name; }
			std::string ProgramName() const  { return its_program_filename; }
			std::string ProjectFolder() const  { return its_dir_pathname; }
			ProductType Product() const  { return its_product_type; }
			
			bool HasPrecompiledHeader() const  { return !its_precompiled_header_source_path.empty(); }
			
			const std::string& PrecompiledHeaderSource() const  { return its_precompiled_header_source_path; }
			
			const std::vector< std::string >& AllUsedProjects() const  { return its_used_project_names; }
			
			const std::vector< std::string >& SearchDirs()      const  { return its_search_dir_pathnames;      }
			const std::vector< std::string >& SourceFileSpecs() const  { return its_source_paths; }
			const std::vector< std::string >& ToolSourceFiles() const  { return its_tool_source_filenames; }
			
			std::vector< std::string > LibImports()    const  { return its_lib_import_specs; }
			std::vector< std::string > Frameworks()    const  { return its_framework_names; }
			std::vector< std::string > UsedRezFiles()  const  { return its_rez_filenames; }
			std::vector< std::string > UsedRsrcFiles() const  { return its_rsrc_filenames; }
			
			std::string CreatorCode() const  { return its_creator_code; }
			
			const std::vector< std::string >& Sources() const  { return its_source_file_pathnames; }
			
			void Study();
			
			std::string FindInclude( const std::string& includePath );
	};
	
}

#endif

