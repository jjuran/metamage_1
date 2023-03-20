/*	==========
 *	Project.cc
 *	==========
 */

#include "A-line/Project.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>
#include <set>

// Standard C/C++
#include <cstdio>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/string/concat.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/basename.hh"
#include "poseven/functions/stat.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Exceptions.hh"
#include "A-line/DeepFiles.hh"
#include "A-line/Locations.hh"
#include "A-line/ProjectCatalog.hh"
#include "A-line/ProjectCommon.hh"
#include "A-line/ProjectConfig.hh"
#include "A-line/SourceDotList.hh"


namespace poseven
{
	
#ifdef __MWERKS__
	
	static plus::string realpath( const char* pathname )
	{
		plus::var_string result;
		
		ssize_t size = 128;
		
		while ( size > result.size() )
		{
			char* p = result.reset( size );
			
			size = _realpath( pathname, p, result.size() );
			
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
	
	inline plus::string realpath( const plus::string& pathname )
	{
		return realpath( pathname.c_str() );
	}
	
#else
	
	inline const plus::string& realpath( const plus::string& pathname )
	{
		return pathname;
	}
	
	
	
#endif
	
}


namespace tool
{
	
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	const StringVector& get_values( const ConfData& config, const plus::string& key )
	{
		ConfData::const_iterator it = config.find( key );
		
		if ( it == config.end () )
		{
			static StringVector null;
			
			return null;
		}
		
		return it->second;
	}
	
	static inline
	const plus::string& get_first( const StringVector& v )
	{
		if ( v.empty() )
		{
			return plus::string::null;
		}
		
		return v[ 0 ];
	}
	
	static
	const plus::string& get_first( const ConfData& config, const plus::string& key )
	{
		return get_first( get_values( config, key ) );
	}
	
	
	plus::string Project::FindInclude( const plus::string& include_path ) const
	{
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = its_search_dir_pathnames.begin();  it != its_search_dir_pathnames.end();  ++it )
		{
			const plus::string& search_dir_pathname = *it;
			
			plus::string include_pathname = search_dir_pathname / include_path;
			
			if ( io::file_exists( include_pathname ) )
			{
				return include_pathname;
			}
		}
		
		return "";
	}
	
	const plus::string& Project::FindIncludeRecursively( const plus::string& include_path ) const
	{
		vxo::Box* box = &its_include_map.find_or_insert( include_path );
		
		vxo::String*& result = *(vxo::String**) &box;
		
		if ( box->undefined() )
		{
			*result = FindInclude( include_path );
			
			const StringVector& project_names = AllUsedProjects();
			
			for ( size_t i = 0;  i < project_names.size();  ++i )
			{
				if ( ! result->empty() )
				{
					break;
				}
				
				const plus::string& name = project_names[ i ];
				
				const Project& used_project = GetProject( name, its_platform );
				
				// for searching only directly used projects, call recursive
				// for searching all used projects, call non-recursive
				plus::string found = used_project.FindInclude( include_path );
				
				// GetProject() may have invalidated our pointer; reassign it.
				result = &its_include_map.assign( include_path, found );
			}
		}
		
		return *result;
	}
	
	plus::string Project::FindResourceFile( const plus::string& filespec ) const
	{
		if ( const char* colon = std::strchr( filespec.c_str(), ':' ) )
		{
			plus::string project_name( filespec.c_str(), colon );
			
			const char* filename = colon + 1;
			
			return GetProject( project_name, its_platform ).FindResourceFile( filename );
		}
		
		try
		{
			plus::string file = its_dir_pathname / "Rez" / filespec;
			
			(void) p7::stat( file );  // throw if nonexistent
			
			return file;
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
	
	static plus::string FindSearchDir( const plus::string& cwdPath, const plus::string& pathname )
	{
		try
		{
			plus::string dirPath = pathname[0] == '/' ? pathname
			                     : pathname[0] == '~' ? home_dir_pathname() / pathname.substr( 2 )
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
	
	
	static inline
	bool ends_with( const plus::string& s, const char* tail, size_t len )
	{
		const char*  data = s.data();
		const size_t size = s.size();
		
		return size >= len  &&  memcmp( data + size - len, tail, len ) == 0;
	}
	
	static ProductType ReadProduct( const plus::string& productName )
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
		else if ( productName == "DA" )
		{
			return productDA;
		}
		else if ( productName == "driver" )
		{
			return productDriver;
		}
		
		return productNotBuilt;
	}
	
	static void GetDirectlyUsedProjectsFromConfig( const ConfData& config, StringVector& used )
	{
		// Figure out which projects we use
		const StringVector& projects_via_USE  = get_values( config, "use"  );
		const StringVector& projects_via_USES = get_values( config, "uses" );
		
		used.insert( used.end(),
		             projects_via_USES.begin(),
		             projects_via_USES.end() );
		
		used.insert( used.end(),
		             projects_via_USE.begin(),
		             projects_via_USE.end() );
	}
	
	static Project global_empty_project;
	
	static const Project& GetUsedProject( const plus::string&  user_name,
	                                      const plus::string&  used_name,
	                                      Platform             platform )
	{
		bool optional = used_name.back() == '?';
		
		size_t name_size = used_name.size() - optional;
		
		plus::string lookup_name = used_name.substr( 0, name_size );
		
		try
		{
			return GetProject( lookup_name, platform, optional );
		}
		catch ( const NoSuchProject& )
		{
			if ( used_name[0] == '_' )
			{
				return global_empty_project;
			}
			
			throw NoSuchUsedProject( user_name, lookup_name );
		}
		
		// Not reached, but MWC 2.4.1 is unable to determine this
		throw;
	}
	
	static
	StringVector GetAllUsedProjects( const plus::string&  project_name,
	                                 Platform             platform,
	                                 const StringVector&  used_project_names )
	{
		StringVector all_used_project_names;
		
		std::set< plus::string > set_of_all_used_project_names;
		
		typedef StringVector::const_iterator Iter;
		
		// For each project named in the 'uses' directive:
		for ( Iter it = used_project_names.begin();  it != used_project_names.end();  ++it )
		{
			// Recursively creates projects
			const Project& used = GetUsedProject( project_name, *it, platform );
			
			// Find out which projects it uses
			const StringVector& subUsed = used.AllUsedProjects();
			
			// For each project even indirectly used by this one:
			for ( Iter it2 = subUsed.begin();  it2 != subUsed.end();  ++it2 )
			{
				const plus::string& name = *it2;
				
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
	
	static
	StringVector get_search_dir_pathnames( const StringVector&  search_directives,
	                                       const plus::string&  project_dir_pathname )
	{
		StringVector result( search_directives.size() );
		
		for ( size_t i = 0;  i < search_directives.size();  ++i )
		{
			result.push_back( FindSearchDir( project_dir_pathname, search_directives[ i ] ) );
		}
		
		return result;
	}
	
	
	static bool IsCompilableExtension( const plus::string& ext )
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
	
	static bool IsCompilableFilename( const plus::string& filename )
	{
		std::size_t lastDot = filename.find_last_of( "." );
		
		return    lastDot != plus::string::npos
		       && IsCompilableExtension( filename.substr( lastDot ) )
		       && !match_backwards( filename.data() + lastDot, STR_LEN( " copy" ) );
	}
	
	
	static plus::string FindSourceFileInDirs( const plus::string& relative_path, const StringVector& search_dirs )
	{
		typedef StringVector::const_iterator dir_iter;
		
		for ( dir_iter it = search_dirs.begin();  it != search_dirs.end();  ++it )
		{
			const plus::string& dir = *it;
			
			ASSERT( !dir.empty() );
			
			const bool has_trailing_slash = dir.end()[ -1 ] == '/';
			
			const char* sentinel = &"//"[ has_trailing_slash ];
			
			// dir has trailing slash, add another for sentinel
			plus::string result = dir + sentinel + relative_path;
			
			if ( io::item_exists( result ) )
			{
				return result;
			}
		}
		
		std::fprintf( stderr, "Missing source file %s\n", relative_path.c_str() );
		
		p7::throw_errno( ENOENT );
		
		throw;  // Not reached
	}
	
	
	static void get_source_data( const plus::string&  project_dir,
	                             const StringVector&  source_paths,
	                             StringVector&        source_file_search_dirs,
	                             StringVector&        source_file_pathnames )
	{
		if ( !source_paths.empty() )
		{
			// 'sources' directive specifies source files or source list files.
			typedef StringVector::const_iterator str_iter;
			
			for ( str_iter it = source_paths.begin();  it != source_paths.end();  ++it )
			{
				const plus::string& project_relative_path = *it;
				
				ASSERT( project_dir[ project_dir.size() - 1 ] == '/' );
				
				// project_dir has trailing slash, add another for sentinel
				plus::string absolute_path = project_dir + "/" + project_relative_path;
				
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
	
	static
	StringVector find_sources( const StringVector&  source_filenames,
	                           const StringVector&  search_dir_pathnames )
	{
		StringVector result;
		
		// We have filenames -- now, find them
		
		typedef StringVector::const_iterator str_iter;
		
		for ( str_iter it = source_filenames.begin();  it != source_filenames.end();  ++it )
		{
			const plus::string& source_filename = *it;
			
			result.push_back( FindSourceFileInDirs( source_filename, search_dir_pathnames ) );
		}
		
		return result;
	}
	
	static
	void list_sources( const StringVector&  source_dirs,
	                   StringVector&        output,
	                   bool                 source_only_search )
	{
		// Enumerate our source files
		// FIXME:  Doesn't deal with duplicates
		
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = source_dirs.begin();  it != source_dirs.end();  ++it )
		{
			const plus::string& source_dir = *it;
			
			StringVector deepSources = DeepFiles( source_dir, &IsCompilableFilename );
			
			if ( !source_only_search )
			{
				typedef StringVector::iterator Iter;
				
				for ( Iter it = deepSources.begin();  it != deepSources.end();  ++ it )
				{
					plus::string& pathname = *it;
					
					plus::var_string mutable_path = pathname.move();
					
					mutable_path.insert( mutable_path.begin() + source_dir.size(), '/' );
					
					pathname = mutable_path.move();
				}
			}
			
			output.insert( output.end(),
			               deepSources.begin(),
			               deepSources.end() );
		}
	}
	
	static bool filename_belongs( const plus::string& source_path, const StringVector& tools )
	{
		return std::find( tools.begin(),
		                  tools.end(),
		                  p7::basename( source_path ) ) != tools.end();
	}
	
	class filename_belonging_check
	{
		private:
			const StringVector& its_tools;
		
		public:
			filename_belonging_check( const StringVector& tools )
			:
				its_tools( tools )
			{
			}
			
			bool operator()( const plus::string& source_path ) const
			{
				return filename_belongs( source_path, its_tools );
			}
	};
	
	static std::size_t partition_sources( const StringVector&  tool_filenames,
	                                      StringVector&        source_paths )
	{
		std::size_t n_tools = std::partition( source_paths.begin(),
		                                      source_paths.end(),
		                                      filename_belonging_check( tool_filenames )
		                                    ) - source_paths.begin();
		
		return n_tools;
	}
	
	
	static const ConfData empty_conf_data = ConfData();
	
	Project::Project()
	:
		its_config_data( empty_conf_data ),
		its_program_filename  ( plus::string::null ),
		its_prefix_source_path( plus::string::null )
	{
	}
	
	Project::Project( const plus::string&  proj,
	                  Platform             platform,
	                  const plus::string&  project_dir,
	                  const ConfData&      conf_data )
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
		const plus::string& product_name = get_first( conf_data, "product" );
		
		its_product_type = ReadProduct( product_name );
		
		StringVector used_project_names;
		
		switch ( its_product_type )
		{
			case productApplication:
			case productTool:
			case productToolkit:
			case productINIT:
				used_project_names.push_back( "__" + product_name );
				break;
			
			default:
				break;
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
			its_creator_code.assign( &its_creator_code[ 1 ], 4 );
		}
		
		if ( !its_source_file_pathnames.empty() )
		{
			return;
		}
		
		// Try a Source.list file
		plus::string source_list = SourceDotListFile( its_dir_pathname );
		
		if ( io::item_exists( source_list ) )
		{
			StringVector listed_sources;
			
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
