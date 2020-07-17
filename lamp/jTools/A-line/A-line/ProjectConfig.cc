/*	================
 *	ProjectConfig.cc
 *	================
 */

#include "A-line/ProjectConfig.hh"

// Standard C++
#include <algorithm>

// Standard C/C++
#include <cstdio>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/basename.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Locations.hh"
#include "A-line/Platform.hh"
#include "A-line/ProjectCatalog.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	struct c_string_less
	{
		bool operator()( const char* a, const char* b )
		{
			return std::strcmp( a, b ) < 0;
		}
	};
	
	static bool DirectiveIsRecognized( const plus::string& directive )
	{
		char const* const recognized[] =
		{
			"arch",
			"creator",
			"desc",
			"developer",
			"distributor",
			"frameworks",
			"imports",
			"name",
			"platform",
			"precompile",
			"product",
			"program",
			"require",
			"rez",
			"runtime",
			"search",
			"sources",
			"subprojects",
			"tools",
			"use",
			"uses",
			"version",
			"website",
			"~"
		};
		
		std::size_t length = sizeof recognized / sizeof (const char*);
		
		char const* const* end = recognized + length;
		
		char const* const* edge = std::lower_bound( recognized + 0,
		                                            end,
		                                            directive.c_str(),
		                                            c_string_less() );
		
		bool found = edge != end && directive == *edge;
		
		return found;
	}
	
	static std::map< plus::string, Platform > MakePlatformMap()
	{
		std::map< plus::string, Platform > map;
		
		map[ "68k" ] = arch68K;
		map[ "ppc" ] = archPPC;
		map[ "x86" ] = archX86;
		
		map[ "a4"     ] = runtimeA4CodeResource;
		map[ "a5"     ] = runtimeA5CodeSegments;
		map[ "cfm"    ] = runtimeCodeFragments;
		map[ "mach-o" ] = runtimeMachO;
		
		map[ "blue"    ] = apiMacBlue;
		map[ "classic" ] = apiMacBlue;
		map[ "carbon"  ] = apiMacCarbon;
		
		map[ "mac"  ] = platformMac;
		map[ "unix" ] = platformUnix;
		
		return map;
	}
	
	static void SetPlatformInfo( PlatformDemands      *cumulative_demands,
	                             const plus::string&   new_spec )
	{
		static std::map< plus::string, Platform > map = MakePlatformMap();
		
		if ( new_spec.empty() )
		{
			return;
		}
		
		const bool inverted = new_spec[0] == '!';
		
		plus::string new_prohibition;
		
		if ( inverted )
		{
			new_prohibition.assign( &new_spec[1], &*new_spec.end() );
		}
		
		const plus::string& platform_name = inverted ? new_prohibition : new_spec;
		
		Platform platform = map[ platform_name ];
		
		if ( platform == Platform() )
		{
			throw NoSuchPlatform( platform_name );
		}
		
		PlatformDemands new_demands = PlatformDemands( platform, Platform() );
		
		if ( inverted )
		{
			new_demands = -new_demands;
		}
		
		*cumulative_demands |= new_demands;
		
		cumulative_demands->Verify();
		
		/*
			// Only classic Toolbox is supported on 68K
			// A4 implies 68K (where only classic Toolbox is available)
			// A5 implies 68K (where only classic Toolbox is available)
			// Only Carbon is supported for Mac OS X
		*/
	}
	
	static PlatformDemands MakePlatformInfo( const std::vector< plus::string >& infos )
	{
		PlatformDemands result;
		
		for ( size_t i = 0;  i < infos.size();  ++i )
		{
			SetPlatformInfo( &result, infos[ i ] );
		}
		
		return result;
	}
	
	
	static std::vector< plus::string > global_subprojects;
	
	static plus::string DescendPathToDir( const plus::string& dir, const plus::string& path )
	{
		return dir / path;
	}
	
	static bool ends_with( const plus::string& string, const char* substring, std::size_t length )
	{
		return std::equal( string.end() - length, string.end(), substring );
	}
	
	static plus::string get_project_dir_from_config_file( const plus::string& config_pathname )
	{
		plus::string config_dir = io::get_preceding_directory( config_pathname );
		
		const bool has_confd = ends_with( config_dir, STR_LEN( "A-line.confd/" ) );
		
		return has_confd ? io::get_preceding_directory( config_dir )
		                 :                              config_dir;
	}
	
	static void AddConfigFile( const plus::string& config_pathname, const ConfData& conf )
	{
		plus::string project_dir = get_project_dir_from_config_file( config_pathname );
		
		typedef ConfData::const_iterator const_iterator;
		
		const_iterator it = conf.find( "name" );
		
		plus::string project_name = it != conf.end() ? it->second[ 0 ]
		                                             : p7::basename( project_dir );
		
		ASSERT( project_name.back() != '/' );
		
		it = conf.find( "platform" );
		
		PlatformDemands demands = it != conf.end() ? MakePlatformInfo( it->second )
		                                           : PlatformDemands();
		
		try
		{
			AddProjectConfigFile( project_name,
			                      demands,
			                      ProjectConfig( config_pathname, project_dir, conf ) );
		}
		catch ( const NoSuchPlatform& e )
		{
			std::fprintf( stderr, "No such platform '%s' in %s\n",
			                                         e.name.c_str(),
			                                                config_pathname.c_str() );
			
			throw p7::exit_failure;
		}
		
	}
	
	static bool ends_with( const char* a, size_t a_size, const char* b, size_t b_size )
	{
		const ssize_t diff_size = a_size - b_size;
		
		return diff_size >= 0  &&  memcmp( a + diff_size, b, b_size ) == 0;
	}
	
	static void AddPendingConfigFile( const plus::string& filePath )
	{
		if ( !ends_with( filePath.data(), filePath.size(), STR_LEN( ".conf" ) ) )
		{
			return;
		}
		
		DotConfData data;
		ReadProjectDotConf( filePath.c_str(), data );
		ConfData conf = MakeConfData( data );
		
		AddConfigFile( filePath, conf );
		
		plus::string proj_dir = get_project_dir_from_config_file( filePath );
		
		std::vector< plus::string >& conf_subprojs = conf[ "subprojects" ];
		
		typedef std::vector< plus::string >::const_iterator It;
		
		for ( It it = conf_subprojs.begin();  it != conf_subprojs.end();  ++it )
		{
			global_subprojects.push_back( DescendPathToDir( proj_dir, *it ) );
		}
	}
	
	void AddPendingSubproject( const plus::string& dir )
	{
		std::vector< plus::string > configs;
		std::vector< plus::string > folders;
		
		ScanDirForProjects( dir,
		                    std::back_inserter( configs ),
		                    std::back_inserter( folders ) );
		
		std::vector< plus::string >& subprojects = global_subprojects;
		
		subprojects.insert( subprojects.end(), folders.begin(), folders.end() );
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = configs.begin();  it != configs.end();  ++it )
		{
			AddPendingConfigFile( *it );
		}
	}
	
	bool AddPendingSubprojects()
	{
		std::vector< plus::string > subprojects;
		
		using std::swap;
		
		swap( subprojects, global_subprojects );
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = subprojects.begin();  it != subprojects.end();  ++it )
		{
			AddPendingSubproject( *it );
		}
		
		return subprojects.size() > 0;
	}
	
	class ConfDataMaker
	{
		private:
			ConfData& conf;
		
		public:
			ConfDataMaker( ConfData& conf ) : conf( conf )  {}
			
			void operator()( const DotConfLine& line ) const
			{
				if ( !DirectiveIsRecognized( line.key ) )
				{
					std::fprintf( stderr,
					              "Unrecognized directive '%s' in project config\n",
					                                       line.key.c_str() );
				}
				
				std::vector< plus::string >& conf_key = conf[ line.key ];
				
				conf_key.insert( conf_key.end(), line.values.begin(), line.values.end() );
			}
	};
	
	ConfData MakeConfData( const DotConfData& data )
	{
		ConfData conf;
		
		std::for_each( data.begin(),
		               data.end(),
		               ConfDataMaker( conf ) );
		
		return conf;
	}
	
}
