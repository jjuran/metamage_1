/*	================
 *	ProjectConfig.cc
 *	================
 */

#include "CompileDriver/ProjectConfig.hh"

// Standard C++
#include <list>
#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/ResourceTransfer.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// CompileDriver
#include "CompileDriver/Platform.hh"
#include "CompileDriver/ProjectCatalog.hh"
#include "CompileDriver/Subprojects.hh"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	
	namespace ext = N::STLExtensions;
	
	
	static void SetPlatformInfo( N::ResourceTransfer< Platform > platform, const std::string& info )
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
			
			// Only Carbon and PowerPC are supported for Mac OS X
			platform->arch = MergeAttributes( platform->arch, archPPC      );
			platform->api  = MergeAttributes( platform->api,  apiMacCarbon );
		}
		
		if ( info == "classic" )
		{
			platform->api = MergeAttributes( platform->api, apiMacToolbox );
		}
		else if ( info == "carbon" )
		{
			platform->api = MergeAttributes( platform->api, apiMacCarbon );
			
			// Carbon is only supported on PowerPC
			platform->arch = MergeAttributes( platform->arch, archPPC );
		}
	}
	
	static Platform MakePlatformInfo( const std::vector< std::string >& infos )
	{
		Platform result;
		
		std::for_each( infos.begin(),
		               infos.end(),
		               std::bind1st( N::PtrFun( SetPlatformInfo ),
		                             N::ResourceTransfer< Platform >( result ) ) );
		
		return result;
	}
	
	ProjectData::ProjectData( const N::FSDirSpec& folder, const ConfData& conf )
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
	
	void AddProjectConfig( const ProjName& projName,
	                       const N::FSDirSpec& folder,
	                       const ConfData& conf )
	{
		gProjectDataList.push_back( ProjectData( folder, conf ) );
		gProjectMap[ projName ].push_back( &gProjectDataList.back() );
	}
	
	static N::FSDirSpec DescendPathToDir( const N::FSDirSpec& dir, const std::string& path )
	{
		using namespace N::Operators;
		
		typedef std::string::size_type size_type;
		const size_type npos = std::string::npos;
		
		N::FSDirSpec result = dir;
		size_type start = 0;
		
		while ( start != npos )
		{
			size_type stop = path.find( '/', start );
			result = result << path.substr( start, stop - start );
			start = stop == npos ? npos : stop + 1;
		}
		
		return result;
	}
	
	static void AddPendingConfigFile( const FSSpec& file )
	{
		std::string filename = N::Convert< std::string >( N::FSpGetName( file ) );
		std::string extension = ".conf";
		
		std::string::difference_type rootSize = filename.size() - extension.size();
		
		if ( rootSize <= 0  ||  filename.substr( rootSize, std::string::npos ) != extension )
		{
			return;
		}
		
		N::FSDirSpec parent = N::FSpGetParent( file );
		std::string name = N::Convert< std::string >( N::FSDirGetName( parent ) );
		
		if ( name == "A-line.confd" )
		{
			parent = N::FSDirGetParent( parent );
			name = N::Convert< std::string >( N::FSDirGetName( parent ) );
		}
		
		DotConfData data;
		ReadProjectDotConf( file, data );
		ConfData conf = MakeConfData( data );
		
		typedef ConfData::const_iterator const_iterator;
		
		const_iterator found = conf.find( "name" );
		if ( found != conf.end() )
		{
			name = found->second[ 0 ];  // 'name' directive overrides folder name
		}
		
		AddProjectConfig( name, parent, conf );
		
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
	
	const N::FSDirSpec& GetProjectFolder( const ProjName& projName, const Platform& targetPlatform )
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

