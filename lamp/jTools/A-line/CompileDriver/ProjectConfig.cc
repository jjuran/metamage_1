/*	================
 *	ProjectConfig.cc
 *	================
 */

#include "CompileDriver/ProjectConfig.hh"

// Standard C++
#include <list>
#include <string>
#include <vector>

// Nucleus
#include "Nucleus/ResourceTransfer.h"

// GetPathname
#include "GetPathname.hh"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectCatalog.hh"
#include "CompileDriver/Subprojects.hh"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	namespace ext = N::STLExtensions;
	
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
			"options",
			"platform",
			"precompile",
			"product",
			"program",
			"rez",
			"rsrc",
			"runtime",
			"search",
		//	"sources",
			"subprojects",
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
	
	static void SetPlatformInfo( NN::ResourceTransfer< Platform > platform, const std::string& info )
	{
		if ( info == "68k" )
		{
			platform->arch = MergeAttributes( platform->arch, arch68K );
			
			// Only classic Toolbox is supported on 68K
			platform->api = MergeAttributes( platform->api, apiMacToolbox );
		}
		else if ( info == "ppc" )
		{
			platform->arch = MergeAttributes( platform->arch, archPPC );
		}
		else if ( info == "x86" )
		{
			platform->arch = MergeAttributes( platform->arch, archX86 );
		}
		
		if ( info == "a4" )
		{
			platform->runtime = MergeAttributes( platform->runtime, runtimeA4CodeResource );
			
			// Implies 68K (where only classic Toolbox is available)
			platform->arch = MergeAttributes( platform->arch, arch68K       );
			platform->api  = MergeAttributes( platform->api,  apiMacToolbox );
		}
		else if ( info == "a5" )
		{
			platform->runtime = MergeAttributes( platform->runtime, runtimeA5CodeSegments );
			
			// Implies 68K (where only classic Toolbox is available)
			platform->arch = MergeAttributes( platform->arch, arch68K       );
			platform->api  = MergeAttributes( platform->api,  apiMacToolbox );
		}
		else if ( info == "cfm" )
		{
			platform->runtime = MergeAttributes( platform->runtime, runtimeCodeFragments );
		}
		else if ( info == "mach-o" )
		{
			platform->runtime = MergeAttributes( platform->runtime, runtimeMachO );
			
			// Only Carbon is supported for Mac OS X
			platform->api  = MergeAttributes( platform->api,  apiMacCarbon );
		}
		
		if ( info == "classic" )
		{
			platform->api = MergeAttributes( platform->api, apiMacToolbox );
		}
		else if ( info == "carbon" )
		{
			platform->api = MergeAttributes( platform->api, apiMacCarbon );
		}
	}
	
	static Platform MakePlatformInfo( const std::vector< std::string >& infos )
	{
		Platform result;
		
		std::for_each( infos.begin(),
		               infos.end(),
		               std::bind1st( N::PtrFun( SetPlatformInfo ),
		                             NN::ResourceTransfer< Platform >( result ) ) );
		
		return result;
	}
	
	ProjectData::ProjectData( const std::string& folder, const ConfData& conf )
	:
		folder  ( folder ),
		confData( conf   ),
		platform( MakePlatformInfo( confData[ "platform" ] ) )
	{
		
	}
	
	typedef std::list< ProjectData > ProjectDataList;
	
	typedef std::vector< ProjectData* > ProjectAlternatives;
	typedef std::map< ProjName, ProjectAlternatives > ProjectMap;
	
	static ProjectDataList  gProjectDataList;
	static ProjectMap       gProjectMap;
	
	static void AddProjectConfig( const ProjName&     projName,
	                              const std::string&  folder,
	                              const ConfData&     conf )
	{
		gProjectDataList.push_back( ProjectData( folder, conf ) );
		gProjectMap[ projName ].push_back( &gProjectDataList.back() );
	}
	
	static N::FSDirSpec DescendPathToDir( const N::FSDirSpec& dir, const std::string& path )
	{
		N::FSDirSpec result = dir;
		std::size_t start = 0;
		
		while ( start != path.npos )
		{
			std::size_t stop = path.find( '/', start );
			
			result /= path.substr( start, stop - start );
			
			start = stop == path.npos ? path.npos : stop + 1;
		}
		
		return result;
	}
	
	/*
	static std::string DescendPathToDir( const std::string& dir, const std::string& path )
	{
		return result / path;
	}
	*/
	
	static void AddPendingConfigFile( const FSSpec& file )
	{
		std::string filename = io::get_filename_string( file );
		std::string extension = ".conf";
		
		std::string::difference_type rootSize = filename.size() - extension.size();
		
		if ( rootSize <= 0  ||  filename.substr( rootSize, std::string::npos ) != extension )
		{
			return;
		}
		
		N::FSDirSpec  parent = io::get_preceding_directory( file   );
		std::string   name   = io::get_filename_string    ( parent );
		
		if ( name == "A-line.confd" )
		{
			parent = io::get_preceding_directory( parent );
			name   = io::get_filename_string    ( parent );
		}
		
		std::string pathname = GetPOSIXPathname( file );
		
		DotConfData data;
		ReadProjectDotConf( pathname, data );
		ConfData conf = MakeConfData( data );
		
		typedef ConfData::const_iterator const_iterator;
		
		const_iterator found = conf.find( "name" );
		if ( found != conf.end() )
		{
			name = found->second[ 0 ];  // 'name' directive overrides folder name
		}
		
		AddProjectConfig( name, GetPOSIXPathname( parent ), conf );
		
		std::for_each( conf[ "subprojects" ].begin(),
		               conf[ "subprojects" ].end(),
		               ext::compose1( N::PtrFun( AddSubproject ),
		                              std::bind1st( N::PtrFun( DescendPathToDir ),
		                                            parent ) ) );
		
	}
	
	void AddPendingSubproject( const N::FSDirSpec& dir )
	{
		std::vector< FSSpec > configs;
		ScanDirForProjects( dir, std::back_inserter( configs ) );
		
		std::for_each( configs.begin(),
		               configs.end(),
		               N::PtrFun( AddPendingConfigFile ) );
		
		
	}
	
	static void AddPendingSubprojects()
	{
		std::vector< N::FSDirSpec > subprojects;
		
		std::swap( subprojects, Subprojects() );
		
		std::for_each( subprojects.begin(),
		               subprojects.end(),
		               N::PtrFun( AddPendingSubproject ) );
		
	}
	
	static void RecursivelyAddPendingSubprojects()
	{
		while ( Subprojects().size() > 0 )
		{
			AddPendingSubprojects();
		}
	}
	
	static bool ProjectPlatformIsCompatible( const ProjectData* projectData,
	                                         const Platform& target )
	{
		const Platform& platform = projectData->platform;
		
		if (    platform.arch != archUnspecified
		     && target.arch   != archUnspecified
		     && platform.arch != target.arch )
		{
			return false;
		}
		
		if (    platform.runtime != runtimeUnspecified
		     && target.runtime   != runtimeUnspecified
		     && platform.runtime != target.runtime )
		{
			return false;
		}
		
		if (    platform.api != apiUnspecified
		     && target.api   != apiUnspecified
		     && platform.api != target.api )
		{
			return false;
		}
		
		return true;
	}
	
	const ProjectData& GetProjectData( const ProjName& projName, const Platform& targetPlatform )
	{
		typedef ProjectMap::const_iterator name_iterator;
		
		name_iterator foundName = gProjectMap.find( projName );
		
		if ( foundName == gProjectMap.end() )
		{
			RecursivelyAddPendingSubprojects();
			foundName = gProjectMap.find( projName );
			
			if ( foundName == gProjectMap.end() )
			{
				throw NoSuchProject( projName );
			}
		}
		
		const ProjectAlternatives& alternatives = foundName->second;
		
		typedef ProjectAlternatives::const_iterator project_iterator;
		
		project_iterator foundProject = std::find_if( alternatives.begin(),
		                                              alternatives.end(),
		                                              std::bind2nd( N::PtrFun( ProjectPlatformIsCompatible ),
		                                                            targetPlatform ) );
		
		if ( foundProject ==  alternatives.end() )
		{
			// FIXME:  Indicate that there are projects with this name,
			// but that they're not compatible with the current target
			throw NoSuchProject( projName );
		}
		
		return **foundProject;
	}
	
	const std::string& GetProjectFolder( const ProjName& projName, const Platform& targetPlatform )
	{
		return GetProjectData( projName, targetPlatform ).folder;
	}
	
	const ConfData& GetProjectConfig( const ProjName& projName, const Platform& targetPlatform )
	{
		return GetProjectData( projName, targetPlatform ).confData;
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

