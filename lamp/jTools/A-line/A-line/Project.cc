/*	==========
 *	Project.cc
 *	==========
 */

#include "A-line/Project.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <set>
#include <vector>

// Standard C/C++
#include <cstdio>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/pointer_to_function.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/stat.hh"

// pfiles
#include "pfiles/common.hh"

// CompileDriver
#include "CompileDriver/ProjectConfig.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Exceptions.hh"
#include "A-line/DeepFiles.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/SourceDotList.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace poseven
{
	
#ifdef __MWERKS__
	
	static std::string realpath( const char* pathname )
	{
		std::string result;
		
		ssize_t size = 128;
		
		while ( size > result.size() )
		{
			result.resize( size );
			
			size = _realpath( pathname, &result[0], result.size() );
			
			if ( size == -1 )
			{
				throw_errno( errno );
			}
			
			if ( size < -1 )
			{
				size = ~size;
			}
		}
		
		result.resize( size );
		
		return result;
	}
	
	inline std::string realpath( const std::string& pathname )
	{
		return realpath( pathname.c_str() );
	}
	
#else
	
	inline const std::string& realpath( const std::string& pathname )
	{
		return pathname;
	}
	
	
	
#endif
	
}


namespace tool
{
	
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	const std::vector< std::string >& get_values( const ConfData& config, const std::string& key )
	{
		ConfData::const_iterator it = config.find( key );
		
		if ( it == config.end () )
		{
			static std::vector< std::string > null;
			
			return null;
		}
		
		return it->second;
	}
	
	static const std::string& get_first( const std::vector< std::string >& v )
	{
		if ( v.empty() )
		{
			static std::string null;
			
			return null;
		}
		
		return v[ 0 ];
	}
	
	static inline const std::string& get_first( const ConfData& config, const std::string& key )
	{
		return get_first( get_values( config, key ) );
	}
	
	
	static std::string FindIncludeInFolder( const std::string& folder, std::string includePath )
	{
		std::string file = folder / includePath;
		
		(void) p7::stat( file.c_str() );  // throw if nonexistent
		
		return file;
	}
	
	std::string Project::FindInclude( const std::string& include_path ) const
	{
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = its_search_dir_pathnames.begin();  it != its_search_dir_pathnames.end();  ++it )
		{
			const std::string& search_dir_pathname = *it;
			
			std::string include_pathname = search_dir_pathname / include_path;
			
			if ( io::file_exists( include_pathname ) )
			{
				return include_pathname;
			}
		}
		
		return "";
	}
	
	std::string Project::FindIncludeRecursively( const std::string& include_path ) const
	{
		std::string& result = its_include_map[ include_path ];
		
		if ( result.empty() )
		{
			result = FindInclude( include_path );
			
			const std::vector< std::string >& project_names = AllUsedProjects();
			
			typedef std::vector< std::string >::const_iterator Iter;
			
			for ( Iter it = project_names.begin();  result.empty()  &&  it != project_names.end();  ++it )
			{
				const Project& used_project = GetProject( *it, its_platform );
				
				// for searching only directly used projects, call recursive
				// for searching all used projects, call non-recursive
				result = used_project.FindInclude( include_path );
			}
		}
		
		return result;
	}
	
	std::string Project::FindResourceFile( const std::string& filespec ) const
	{
		if ( const char* colon = std::strchr( filespec.c_str(), ':' ) )
		{
			std::string project_name( filespec.c_str(), colon );
			
			const char* filename = colon + 1;
			
			return GetProject( project_name, its_platform ).FindResourceFile( filename );
		}
		
		try
		{
			return FindIncludeInFolder( its_dir_pathname / "Resources", filespec );
		}
		catch ( ... )
		{
		}
		
		try
		{
			return FindIncludeInFolder( its_dir_pathname, filespec );
		}
		catch ( ... )
		{
		}
		
		std::fprintf( stderr, "A-line: %s: can't find resource file '%s'\n",
		                               its_name.c_str(),             filespec.c_str() );
		
		p7::throw_errno( ENOENT );
		
		// Not reached
		return "";
	}
	
	static std::string FindSearchDir( const std::string& cwdPath, const std::string& pathname )
	{
		try
		{
			std::string dirPath = pathname[0] == '/' ? pathname
			                    : pathname[0] == '~' ? home_dir_pathname() / (pathname.c_str() + 2)
			                    :                      p7::realpath( cwdPath / pathname );
			
			if ( io::directory_exists( dirPath ) )
			{
				return dirPath;
			}
			
			std::fprintf( stderr, "Missing search directory '%s'\n", pathname.c_str() );
			
			throw;
		}
		catch ( ... )
		{
			std::fprintf( stderr, "Unrecognized exception for '%s'\n", pathname.c_str() );
			
			throw;
		}
		
		// Not reached
		return "";
	}
	
	
	static bool ends_with( const std::string& s, const char* tail, unsigned length )
	{
		return s.length() >= length  &&  std::equal( s.end() - length, s.end(), tail );
	}
	
	static ProductType ReadProduct( const std::string& productName )
	{
		if ( productName == "app" )
		{
			return productApplication;
		}
		else if ( productName == "source" )
		{
			return productSource;
		}
		else if ( productName == "dropin" )
		{
			return productDropIn;
		}
		else if ( productName == "lib" )
		{
			return productStaticLib;
		}
		else if ( productName == "shlib" )
		{
			return productSharedLib;
		}
		else if ( ends_with( productName, STR_LEN( "tool" ) ) )
		{
			return productTool;
		}
		else if ( productName == "toolkit" )
		{
			return productToolkit;
		}
		else if ( productName == "init" )
		{
			return productINIT;
		}
		else if ( productName == "driver" )
		{
			return productDriver;
		}
		
		return productNotBuilt;
	}
	
	static void GetDirectlyUsedProjectsFromConfig( const ConfData& config, std::vector< std::string >& used )
	{
		// Figure out which projects we use
		const std::vector< std::string >& projects_via_USE  = get_values( config, "use"  );
		const std::vector< std::string >& projects_via_USES = get_values( config, "uses" );
		
		const std::string& product_name = get_first( config, "product" );
		
		used.insert( used.end(),
		             projects_via_USES.begin(),
		             projects_via_USES.end() );
		
		used.insert( used.end(),
		             projects_via_USE.begin(),
		             projects_via_USE.end() );
	}
	
	static Project global_empty_project;
	
	static const Project& GetUsedProject( const std::string&  user_name,
	                                      const std::string&  used_name,
	                                      Platform            platform )
	{
		try
		{
			return GetProject( used_name, platform );
		}
		catch ( const NoSuchProject& )
		{
			if ( used_name[0] == '_' )
			{
				return global_empty_project;
			}
			
			throw NoSuchUsedProject( user_name, used_name );
		}
		
		// Not reached, but MWC 2.4.1 is unable to determine this
		throw;
	}
	
	static std::vector< std::string > GetAllUsedProjects( const std::string&                 project_name,
	                                                      Platform                           platform,
	                                                      const std::vector< std::string >&  used_project_names )
	{
		std::vector< std::string > all_used_project_names;
		
		std::set< std::string > set_of_all_used_project_names;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		// For each project named in the 'uses' directive:
		for ( Iter it = used_project_names.begin();  it != used_project_names.end();  ++it )
		{
			// Recursively creates projects
			const Project& used = GetUsedProject( project_name, *it, platform );
			
			// Find out which projects it uses
			const std::vector< std::string >& subUsed = used.AllUsedProjects();
			
			// For each project even indirectly used by this one:
			for ( Iter it2 = subUsed.begin();  it2 != subUsed.end();  ++it2 )
			{
				const std::string& name = *it2;
				
				// If it isn't already in the collection,
				if ( set_of_all_used_project_names.count( name ) == 0 )
				{
					// Add it to the collection.
					set_of_all_used_project_names.insert( name );
					
					all_used_project_names.push_back( name );
				}
			}
		}
		
		return all_used_project_names;
	}
	
	static std::vector< std::string > get_search_dir_pathnames( const std::vector< std::string >&  search_directives,
	                                                            const std::string&                 project_dir_pathname )
	{
		std::vector< std::string > result;
		
		result.resize( search_directives.size() );
		
		// Find and record search directories.
		std::transform( search_directives.begin(),
		                search_directives.end(),
		                result.begin(),
		                std::bind1st( plus::ptr_fun( FindSearchDir ),
			                          project_dir_pathname ) );
		
		return result;
	}
	
	
	static bool IsCompilableExtension( const std::string& ext )
	{
		if ( ext == ".c"   )  return true;
		if ( ext == ".cc"  )  return true;
		if ( ext == ".cp"  )  return true;
		if ( ext == ".cpp" )  return true;
		if ( ext == ".c++" )  return true;
		
		return false;
	}
	
	static inline bool match_backwards( const char* a_end, const char* b_begin, std::size_t length )
	{
		return std::equal( a_end - length, a_end, b_begin );
	}
	
	static bool IsCompilableFilename( const std::string& filename )
	{
		std::size_t lastDot = filename.find_last_of( "." );
		
		return    lastDot != std::string::npos
		       && IsCompilableExtension( filename.substr( lastDot, std::string::npos ) )
		       && !match_backwards( filename.c_str() + lastDot, STR_LEN( " copy" ) );
	}
	
	
	static std::string FindSourceFileInDirs( const std::string& relative_path, const std::vector< std::string >& search_dirs )
	{
		typedef std::vector< std::string >::const_iterator dir_iter;
		
		for ( dir_iter it = search_dirs.begin();  it != search_dirs.end();  ++it )
		{
			std::string dir = *it;
			
			if ( dir[ dir.size() - 1 ] != '/' )
			{
				dir += '/';
			}
			
			// dir has trailing slash, add another for sentinel
			std::string result = dir + "/" +  relative_path;
			
			if ( io::item_exists( result ) )
			{
				return result;
			}
		}
		
		std::fprintf( stderr, "Missing source file %s\n", relative_path.c_str() );
		
		p7::throw_errno( ENOENT );
		
		throw;  // Not reached
	}
	
	
	static void get_source_data( const std::string&                 project_dir,
	                             const std::vector< std::string >&  source_paths,
	                             std::vector< std::string >&        source_file_search_dirs,
	                             std::vector< std::string >&        source_file_pathnames )
	{
		if ( !source_paths.empty() )
		{
			// 'sources' directive specifies source files or source list files.
			typedef std::vector< std::string >::const_iterator str_iter;
			
			for ( str_iter it = source_paths.begin();  it != source_paths.end();  ++it )
			{
				const std::string& project_relative_path = *it;
				
				ASSERT( project_dir[ project_dir.size() - 1 ] == '/' );
				
				// project_dir has trailing slash, add another for sentinel
				std::string absolute_path = project_dir + "/" + project_relative_path;
				
				if ( io::directory_exists( absolute_path ) )
				{
					source_file_search_dirs.push_back( absolute_path );
					
					continue;
				}
				
				if ( io::file_exists( absolute_path ) )
				{
					source_file_pathnames.push_back( absolute_path );
					
					continue;
				}
				
				if ( *(absolute_path.end() - 1) == '*' )
				{
					
				}
			}
		}
	}
	
	static std::vector< std::string > find_sources( const std::vector< std::string >&  source_filenames,
	                                                const std::vector< std::string >&  search_dir_pathnames )
	{
		std::vector< std::string > result;
		
		// We have filenames -- now, find them
		
		typedef std::vector< std::string >::const_iterator str_iter;
		
		for ( str_iter it = source_filenames.begin();  it != source_filenames.end();  ++it )
		{
			const std::string& source_filename = *it;
			
			result.push_back( FindSourceFileInDirs( source_filename, search_dir_pathnames ) );
		}
		
		return result;
	}
	
	static void list_sources( const std::vector< std::string >&  source_dirs,
	                          std::vector< std::string >&        output,
	                          bool                               source_only_search )
	{
		// Enumerate our source files
		// FIXME:  Doesn't deal with duplicates
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = source_dirs.begin();  it != source_dirs.end();  ++it )
		{
			const std::string& source_dir = *it;
			
			std::vector< std::string > deepSources = DeepFiles( source_dir, std::ptr_fun( IsCompilableFilename ) );
			
			if ( !source_only_search )
			{
				typedef std::vector< std::string >::iterator Iter;
				
				for ( Iter it = deepSources.begin();  it != deepSources.end();  ++ it )
				{
					std::string& pathname = *it;
					
					pathname.insert( pathname.begin() + source_dir.size(), '/' );
				}
			}
			
			output.insert( output.end(),
			               deepSources.begin(),
			               deepSources.end() );
		}
	}
	
	static bool filename_belongs( const std::string& source_path, const std::vector< std::string >& tools )
	{
		return std::find( tools.begin(),
		                  tools.end(),
		                  io::get_filename( source_path ) ) != tools.end();
	}
	
	static std::size_t partition_sources( const std::vector< std::string >&  tool_filenames,
	                                      std::vector< std::string >&        source_paths )
	{
		std::size_t n_tools = std::partition( source_paths.begin(),
		                                      source_paths.end(),
		                                      std::bind2nd( plus::ptr_fun( &filename_belongs ),
		                                                                    tool_filenames ) ) - source_paths.begin();
		
		return n_tools;
	}
	
	
	Project::Project()
	:
		its_config_data( ConfData() ),
		its_program_filename  ( std::string() ),
		its_prefix_source_path( std::string() )
	{
	}
	
	Project::Project( const std::string&  proj,
	                  Platform            platform,
	                  const std::string&  project_dir,
	                  const ConfData&     conf_data )
	:
		its_name        ( proj            ),
		its_platform    ( platform        ),
		its_dir_pathname( project_dir     ),
		its_config_data ( conf_data       ),
		its_product_type( productNotBuilt ),
		its_program_filename  ( get_first( conf_data, "program"    ) ),
		its_prefix_source_path( get_first( conf_data, "precompile" ) ),
		its_tool_count  ()
	{
		const std::string& product_name = get_first( conf_data, "product" );
		
		its_product_type = ReadProduct( product_name );
		
		std::vector< std::string > used_project_names;
		
		switch ( its_product_type )
		{
			case productApplication:
			case productTool:
			case productToolkit:
			case productINIT:
				used_project_names.push_back( "__" + product_name );
		}
		
		GetDirectlyUsedProjectsFromConfig( conf_data, used_project_names );
		
		// Figure out which projects we use
		its_used_project_names = GetAllUsedProjects( its_name, platform, used_project_names );
		
		// Make sure we're in the list too, and make sure we're last.
		its_used_project_names.push_back( proj );
		
		if ( its_product_type == productNotBuilt )
		{
			return;
		}
		
		its_search_dir_pathnames = get_search_dir_pathnames( get_values( conf_data, "search" ), its_dir_pathname );
		
		if ( its_search_dir_pathnames.empty() )
		{
			its_search_dir_pathnames.push_back( its_dir_pathname );
		}
		
		get_source_data( its_dir_pathname,
		                 get_values( conf_data, "sources" ),
		                 its_source_dir_pathnames,
		                 its_source_file_pathnames );
		
		if ( !ProductGetsBuilt( its_product_type ) )
		{
			return;
		}
		
		//printf("%s recursively uses %d projects.\n", proj.c_str(), allUsedProjects.size());
		
		// The creator code for linked output files.  Mac only.
		its_creator_code = get_first( conf_data, "creator" );
		
		if ( its_creator_code.size() == 6  &&  its_creator_code[0] == '\''  &&  its_creator_code[5] == '\'' )
		{
			its_creator_code = its_creator_code.substr( 1, 4 );
		}
		
		if ( !its_source_file_pathnames.empty() )
		{
			return;
		}
		
		// Try a Source.list file
		std::string source_list = SourceDotListFile( its_dir_pathname );
		
		if ( io::item_exists( source_list ) )
		{
			std::vector< std::string > listed_sources;
			
			ReadSourceDotList( source_list.c_str(), listed_sources );
			
			its_source_file_pathnames = find_sources( listed_sources,
		                                              its_search_dir_pathnames );
		}
		else
		{
			const bool source_only_search = !its_source_dir_pathnames.empty();
			
			list_sources( source_only_search ? its_source_dir_pathnames
		                                     : its_search_dir_pathnames,
		                  its_source_file_pathnames,
		                  source_only_search );
		}
		
		if ( its_product_type == productToolkit )
		{
			its_tool_filenames = get_values( its_config_data, "tools" );
			
			std::sort( its_tool_filenames.begin(), its_tool_filenames.end() );
			
			its_tool_count = partition_sources( its_tool_filenames, its_source_file_pathnames );
		}
	}
	
}

