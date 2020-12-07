/*	=================
 *	ProjectCatalog.cc
 *	=================
 */

#include "A-line/ProjectCatalog.hh"

// Standard C++
#include <vector>

// Iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/pointer_to_function.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Io
#include "io/files.hh"
#include "io/walk.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/basename.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/sequences/directory_contents.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/Exceptions.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	template < class Data >
	struct project_data
	{
		// A map from platform requirements to project data
		typedef std::map< PlatformDemands, Data > platform_map;
		
		// A map from project name to config file set
		typedef std::map< plus::string, platform_map > name_map;
	};
	
	typedef project_data< ProjectConfig >::platform_map  ProjectConfigCandidates;
	typedef project_data< ProjectConfig >::name_map      ProjectCatalog;
	
	
	static ProjectCatalog gProjectCatalog;
	
	
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
	
	
	void AddProjectConfigFile( const plus::string&     name,
	                           const PlatformDemands&  demands,
	                           const ProjectConfig&    config )
	{
		gProjectCatalog[ name ][ demands ] = config;
	}
	
	struct platform_compatibility
	{
		Platform platform;
		
		platform_compatibility() : platform()
		{
		}
		
		platform_compatibility( Platform p ) : platform( p )
		{
		}
		
		bool operator()( const PlatformDemands& demands ) const
		{
			return demands.Test( platform );
		}
		
		template < class value_type >
		bool operator()( const value_type& map_value ) const
		{
			return operator()( map_value.first );
		}
	};
	
	static ProjectConfigCandidates& find_project_config_candidates( const plus::string& project_name )
	{
		ProjectCatalog::iterator it = gProjectCatalog.find( project_name );
		
		if ( it == gProjectCatalog.end() )
		{
			throw NoSuchProject( project_name );
		}
		
		return it->second;
	}
	
	const ProjectConfig& GetProjectConfig( const plus::string& name, Platform targetPlatform )
	{
		ProjectConfigCandidates& candidates = find_project_config_candidates( name );
		
		ProjectConfigCandidates::iterator it = std::find_if( candidates.begin(),
		                                                     candidates.end(),
		                                                     platform_compatibility( targetPlatform ) );
		if ( it == candidates.end() )
		{
			// FIXME:  Indicate that there are projects with this name,
			// but that they're not compatible with the current target
			throw NoSuchProject( name );
		}
		
		ProjectConfig& result = it->second;
		
		if ( result.get_config_data().empty() )
		{
			try
			{
				result.load_config();
			}
			catch ( const p7::errno_t& err )
			{
				if ( err != ENOENT )
				{
					throw;
				}
				
				throw missing_project_config( name, result.get_pathname() );
			}
		}
		
		return result;
	}
	
	static void add_cached_config( const plus::string&     project_name,
	                               const PlatformDemands&  demands,
	                               const plus::string&     pathname )
	{
		ProjectConfig& config = gProjectCatalog[ project_name ][ demands ];
		
		if ( config.get_pathname().empty() )
		{
			config.set_pathname( pathname );
		}
	}
	
	void ProjectConfig::load_config()
	{
		its_project_dir = get_project_dir_from_config_file( its_pathname );
		
		DotConfData data;
		
		ReadProjectDotConf( its_pathname.c_str(), data );
		
		its_config_data = MakeConfData( data );
	}
	
	void ScanDirForProjects( const plus::string&  dirPath,
	                         StringVector&        configs,
	                         StringVector&        folders )
	{
		struct stat dir_stat;
		
		if ( !p7::stat( dirPath, dir_stat )  ||  !p7::s_isdir( dir_stat ) )
		{
			return;
		}
		
		plus::string dirName = p7::basename( dirPath );
		
		if ( dirName[0] == '('  &&  dirName.back() == ')' )
		{
			return;  // skip "(Guarded)" directories
		}
		
		if ( dirName == ".git" )
		{
			return;
		}
		
		if ( dirName == "CVS" )
		{
			return;
		}
		
		plus::string conf = dirPath / "A-line.conf";
		
		if ( io::file_exists( conf ) )
		{
			configs.push_back( conf );
			
			return;
		}
		
		typedef io::directory_contents_traits< plus::string >::container_type directory_container;
		
		plus::string confd = dirPath / "A-line.confd";
		
		const bool has_confd = io::directory_exists( confd );
		
		const plus::string& conf_path = has_confd ? confd : dirPath;
		
		directory_container contents = io::directory_contents( conf_path );
		
		typedef directory_container::const_iterator Iter;
		
		Iter end = contents.end();
		
		for ( Iter it = contents.begin();  it != end;  ++it )
		{
			StringVector& paths = (has_confd ? configs : folders);
			
			paths.push_back( io::path_descent( conf_path, *it ) );
		}
	}
	
	
	void write_catalog_cache( p7::fd_t output )
	{
		typedef ProjectCatalog::const_iterator name_iter;
		
		typedef ProjectConfigCandidates::const_iterator demands_iter;
		
		for ( name_iter the_name = gProjectCatalog.begin();  the_name != gProjectCatalog.end();  ++the_name )
		{
			const plus::string&             name       = the_name->first;
			const ProjectConfigCandidates&  candidates = the_name->second;
			
			for ( demands_iter the_demands = candidates.begin();  the_demands != candidates.end();  ++the_demands )
			{
				const PlatformDemands&  demands = the_demands->first;
				const ProjectConfig&    config  = the_demands->second;
				
				plus::var_string record;
				
				record.reserve( name.size() + config.get_pathname().size() + 2 * 10 + 4 );
				
				record += name;
				
				record += '\t';
				
				record += gear::inscribe_unsigned_decimal( demands.Required  () );
				
				record += '/';
				
				record += gear::inscribe_unsigned_decimal( demands.Prohibited() );
				
				record += '\t';
				
				record += config.get_pathname();
				
				record += '\n';
				
				p7::write( output, record );
			}
		}
	}
	
	void read_catalog_cache( p7::fd_t input_fd )
	{
		text_input::feed feed;
		
		p7::fd_reader reader( input_fd );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string text( *s );
			
			const char* begin = text.c_str();
			
			if ( const char* tab1 = std::strchr( begin, '\t' ) )
			{
				if ( const char* slash = std::strchr( tab1 + 1, '/' ) )
				{
					if ( const char* tab2 = std::strchr( slash + 1, '\t' ) )
					{
						plus::string project_name( begin, tab1 );
						
						const char* requirements = tab1  + 1;
						const char* prohibitions = slash + 1;
						
						plus::string config_pathname( tab2 + 1 );
						
						PlatformDemands demands( (Platform) gear::parse_unsigned_decimal( requirements ),
						                         (Platform) gear::parse_unsigned_decimal( prohibitions ) );
						
						add_cached_config( project_name, demands, config_pathname );
					}
				}
			}
		}
	}
	
}
