/*	================
 *	ProjectConfig.cc
 *	================
 */

#include "CompileDriver/ProjectConfig.hh"

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/ResourceTransfer.h"

// POSeven
#include "POSeven/Pathnames.hh"

// MoreFunctional
#include "FunctionalExtensions.hh"
#include "PointerToFunction.hh"

// Orion
#include "Orion/Main.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace CompileDriver
{
	
	namespace NN = Nucleus;
	
	using namespace io::path_descent_operators;
	
	
	static bool c_string_less( const char* a, const char* b )
	{
		return std::strcmp( a, b ) < 0;
	}
	
	static bool DirectiveIsRecognized( const std::string& directive )
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
			"rez",
			"rsrc",
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
		
		char const* const* edge = std::lower_bound( recognized + 0, end, directive.c_str(), std::ptr_fun( c_string_less ) );
		
		bool found = edge != end && directive == *edge;
		
		return found;
	}
	
	static std::map< std::string, Platform > MakePlatformMap()
	{
		std::map< std::string, Platform > map;
		
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
	
	static void SetPlatformInfo( NN::ResourceTransfer< PlatformDemands > cumulativeDemands, std::string info )
	{
		static std::map< std::string, Platform > map = MakePlatformMap();
		
		bool inverted = false;
		
		if ( info.empty() )
		{
			return;
		}
		else if ( info[0] == '!' )
		{
			info = info.substr( 1, info.npos );
			inverted = true;
		}
		
		Platform platform = map[ info ];
		
		if ( platform == Platform() )
		{
			throw NoSuchPlatform( info );
		}
		
		PlatformDemands infoDemands = PlatformDemands( platform, Platform() );
		
		if ( inverted )
		{
			infoDemands = -infoDemands;
		}
		
		*cumulativeDemands |= infoDemands;
		
		cumulativeDemands->Verify();
		
		/*
			// Only classic Toolbox is supported on 68K
			// A4 implies 68K (where only classic Toolbox is available)
			// A5 implies 68K (where only classic Toolbox is available)
			// Only Carbon is supported for Mac OS X
		*/
	}
	
	static PlatformDemands MakePlatformInfo( const std::vector< std::string >& infos )
	{
		PlatformDemands result;
		
		std::for_each( infos.begin(),
		               infos.end(),
		               std::bind1st( more::ptr_fun( SetPlatformInfo ),
		                             NN::ResourceTransfer< PlatformDemands >( result ) ) );
		
		return result;
	}
	
	
	static std::vector< std::string >& Subprojects()
	{
		static std::vector< std::string > gSubprojects;
		
		return gSubprojects;
	}
	
	static void AddSubproject( const std::string& location )
	{
		Subprojects().push_back( location );
	}
	
	static std::string DescendPathToDir( const std::string& dir, const std::string& path )
	{
		return dir / path;
	}
	
	static void AddPendingConfigFile( const std::string& filePath )
	{
		std::string filename = io::get_filename_string( filePath );
		std::string extension = ".conf";
		
		std::string::difference_type rootSize = filename.size() - extension.size();
		
		if ( rootSize <= 0  ||  filename.substr( rootSize, std::string::npos ) != extension )
		{
			return;
		}
		
		std::string  parent = io::get_preceding_directory( filePath );
		std::string  name   = io::get_filename_string    ( parent   );
		
		if ( name == "A-line.confd" )
		{
			parent = io::get_preceding_directory( parent );
			name   = io::get_filename_string    ( parent );
		}
		
		DotConfData data;
		ReadProjectDotConf( filePath, data );
		ConfData conf = MakeConfData( data );
		
		typedef ConfData::const_iterator const_iterator;
		
		const_iterator found = conf.find( "name" );
		if ( found != conf.end() )
		{
			name = found->second[ 0 ];  // 'name' directive overrides folder name
		}
		
		try
		{
			AddProjectConfigFile( name,
			                      MakePlatformInfo( conf[ "platform" ] ),
			                      ProjectConfig( filePath, parent, conf ) );
		}
		catch ( const NoSuchPlatform& e )
		{
			std::fprintf( stderr, "No such platform '%s' in %s\n", e.name.c_str(), filePath.c_str() );
			
			Orion::ThrowExitStatus( EXIT_FAILURE );
		}
		
		std::for_each( conf[ "subprojects" ].begin(),
		               conf[ "subprojects" ].end(),
		               more::compose1( more::ptr_fun( AddSubproject ),
		                               std::bind1st( more::ptr_fun( DescendPathToDir ),
		                                             parent ) ) );
		
	}
	
	void AddPendingSubproject( const std::string& dir )
	{
		std::vector< std::string > configs;
		std::vector< std::string > folders;
		
		ScanDirForProjects( dir,
		                    std::back_inserter( configs ),
		                    std::back_inserter( folders ) );
		
		std::copy( folders.begin(),
		           folders.end(),
		           std::back_inserter( Subprojects() ) );
		
		std::for_each( configs.begin(),
		               configs.end(),
		               std::ptr_fun( AddPendingConfigFile ) );
	}
	
	static void AddPendingSubprojects()
	{
		std::vector< std::string > subprojects;
		
		std::swap( subprojects, Subprojects() );
		
		std::for_each( subprojects.begin(),
		               subprojects.end(),
		               std::ptr_fun( AddPendingSubproject ) );
		
	}
	
	void RecursivelyAddPendingSubprojects();
	
	void RecursivelyAddPendingSubprojects()
	{
		while ( Subprojects().size() > 0 )
		{
			AddPendingSubprojects();
		}
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
				
				std::copy( line.values.begin(),
				           line.values.end(),
				           std::back_inserter( conf[ line.key ] ) );
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

