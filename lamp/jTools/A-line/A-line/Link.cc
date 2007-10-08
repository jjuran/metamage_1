/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Io
#include "io/io.hh"
#include "io/spray.hh"

// Nucleus
#include "Nucleus/Flattener.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// BitsAndBytes
#include "StringFilters.hh"

// Orion
#include "Orion/StandardIO.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/Locations.hh"
#include "A-line/MWScripter.hh"
#include "A-line/ProjectCommon.hh"


namespace ALine
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	
	static std::string MakeImport( const std::string& file )
	{
		std::string result;
		
		std::string importPath = file;
		
		std::string importName = io::get_filename_string( file );
		
		if ( importName[ 0 ] == '-' )
		{
			importName = importName.substr( 1, importName.npos );
			
			importPath = std::strchr( file.c_str(), '/' ) ? io::get_preceding_directory( file ) / importName
			                                              : importName;
			
			result = "-wi ";
		}
		
		result += q( importPath );
		
		return result;
	}
	
	static std::string MakeEchoedIncludedPOSIXPathname( const std::string& file )
	{
		std::string include = "include ";
		std::string echo    = "echo ";
		
		return echo + q( include + qq( file ) + ";" );
	}
	
	static std::string MakeFramework( const std::string& framework )
	{
		return std::string( "-framework " ) + framework;
	}
	
	
	static std::string FindImport( const std::string& name, const Project& project )
	{
		return name;
	}
	
	static void CopyImports( const ProjName& projName,
	                         std::back_insert_iterator< std::vector< std::string > > inserter )
	{
		Project& project = GetProject( projName );
		
		std::vector< FileName > importNames( project.LibImports() );
		
		std::copy( importNames.begin(),
		           importNames.end(),
		           inserter );
	}
	
	static std::string OptionToSearchProjectForLib( const std::string& projectName )
	{
		Project& project = GetProject( projectName );
		
		if ( project.LibImports().empty() )
		{
			return "";
		}
		
		return "-L'" + project.ProjectFolder() + "'";
	}
	
	static std::string GetImportLocationOptions( const Project& project )
	{
		const std::vector< ProjName >& used = project.AllUsedProjects();
		
		return join( used.begin(),
		             used.end(),
		             " ",
		             std::ptr_fun( OptionToSearchProjectForLib ) );
	}
	
	static std::vector< std::string > GetAllImports( const Project& project )
	{
		const std::vector< ProjName >& used = project.AllUsedProjects();
		
		std::vector< std::string > importFiles;
		
		std::for_each( used.begin(),
		               used.end(),
		               std::bind2nd( more::ptr_fun( CopyImports ),
		                             std::back_inserter( importFiles ) ) );
		
		return importFiles;
	}
	
	static std::string GetImports( const Project& project )
	{
		std::vector< std::string > importFiles( GetAllImports( project ) );
		
		std::string imports;
		
		imports << join( importFiles.begin(),
		                 importFiles.end(),
		                 " ",
		                 std::ptr_fun( MakeImport ) );
		
		return GetImportLocationOptions( project ) + " " + imports;
	}
	
	static std::string GetFrameworks( const Project& project )
	{
		std::vector< FileName > frameworkNames( project.Frameworks() );
		
		if ( frameworkNames.size() == 0 )
		{
			frameworkNames.push_back( "Carbon" );
		}
		
		std::string frameworks = join( frameworkNames.begin(),
		                               frameworkNames.end(),
		                               " ",
		                               std::ptr_fun( MakeFramework ) );
		
		return frameworks;
	}
	
	// FIXME:  Prebuilt libraries do not work for multiple targets.
	// This needs to be a function of target, not project.
	static TargetName gTargetName;
	static std::string gLibraryExtension;
	
	static void GetProjectLib( const Project& project, std::vector< std::string >* const& outUsed )
	{
		if ( project.Product() == productStaticLib )
		{
			std::string libOutput = ProjectLibrariesDirPath( project.Name(), gTargetName );
			
			std::string lib = libOutput / project.Name() + gLibraryExtension;
			
			if ( io::item_exists( lib ) )
			{
				outUsed->push_back( lib );
			}
		}
	}
	
	static std::vector< std::string > GetUsedLibraries( const Project& project, const TargetName& targetName )
	{
		gTargetName = targetName;
		std::vector< std::string > usedLibFiles;
		
		const std::vector< ProjName >& usedProjects = project.AllUsedProjects();
		
		std::for_each( usedProjects.begin(),
		               usedProjects.end() - 1,
		               more::compose1( std::bind2nd( more::ptr_fun( GetProjectLib ),
		                                             &usedLibFiles ),
		                               more::ptr_fun( GetProject ) ) );
		
		return usedLibFiles;
	}
	
	static std::string DirCreate_Idempotent( const std::string& dir )
	{
		if ( !io::item_exists( dir ) )
		{
			int made = mkdir( dir.c_str(), 0700 );
		}
		
		return dir;
	}
	
	static void CreateAppBundle( const std::string& location, const std::string& name )
	{
		std::string package   = DirCreate_Idempotent( location / name );
		std::string contents  = DirCreate_Idempotent(   package  / "Contents" );
		std::string macOS     = DirCreate_Idempotent(     contents / "MacOS" );
		std::string resources = DirCreate_Idempotent(     contents / "Resources" );
	}
	
	static std::string paren( const std::string& s )
	{
		return std::string( "(" ) + s + ")";
	}
	
	void LinkProduct( const Project& project, TargetInfo targetInfo )
	{
		TargetName targetName = MakeTargetName( targetInfo );
		
		const bool gnu = targetInfo.toolkit == toolkitGNU;
		
		gLibraryExtension = gnu ? ".a" : ".lib";
		
		const bool machO = targetInfo.platform & CD::runtimeMachO;
		
		CommandGenerator cmdgen( targetInfo );
		
		std::string linkName = project.ProgramName();
		
		if ( linkName == "" )
		{
			linkName = project.Name();
		}
		
		std::string command = cmdgen.LinkerName();
		
		command << cmdgen.TargetArchitecture();
		
		bool needLibs = true;
		bool needCarbResource = false;
		bool gccSupported = false;
		bool bundle = false;
		
		const CD::Platform carbonCFM = CD::apiMacCarbon | CD::runtimeCodeFragments;
		
		switch ( project.Product() )
		{
			case productStaticLib:
				command = cmdgen.LibraryMakerName();
				linkName = project.Name() + gLibraryExtension;
				needLibs = false;
				gccSupported = true;
				break;
			
			case productApplication:
				command << cmdgen.TargetApplication();
				needCarbResource = (targetInfo.platform & carbonCFM) == carbonCFM;
				gccSupported = true;
				bundle = gnu;
				break;
			
			case productINIT:
				command << cmdgen.MWTargetCodeResource()
				        << cmdgen.ResourceTypeAndID( "INIT", "0" )
				        << cmdgen.OutputType( "INIT" );
				break;
			
			case productDriver:
				command << cmdgen.MWTargetCodeResource()
				        << cmdgen.ResourceTypeAndID( "DRVR", "0" )
				        << cmdgen.ResourceName( "." + project.Name() )
				        << cmdgen.OutputType   ( "INIT" )
				        << cmdgen.OutputCreator( "RSED" )
				        << cmdgen.CustomDriverHeader();
				break;
			
			case productWish:
				gccSupported = true;
				command << cmdgen.TargetCommandLineTool();
				break;
			
			case productSharedLib:
				command << cmdgen.MWTargetSharedLibrary();
				break;
		}
		
		if ( gnu  &&  !gccSupported )
		{
			Io::Err << "Sorry, GCC is not supported for this type of product.\n";
			
			p7::throw_errno( EINVAL );
		}
		
		const bool useAr = gnu  &&  project.Product() == productStaticLib;
		
		if ( !useAr )
		{
			command << cmdgen.LinkerOptions();
		}
		
		std::string objectsDir = ProjectObjectsDirPath( project.Name(), targetName );
		
		std::string outputDir  = project.Product() == productStaticLib ? ProjectLibrariesDirPath( project.Name(), targetName )
		                                                               : ProjectOutputDirPath   ( project.Name(), targetName );
		
		std::string outFile = outputDir / linkName;
		bool outFileExists = io::item_exists( outFile );
		
		time_t outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = !outFileExists;
		
		std::vector< std::string > objectFiles;
		
		std::vector< std::string >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			std::string sourceName = io::get_filename_string( *it );
			
			std::string objectFile = objectsDir / ObjectFileName( sourceName );
			
			needToLink =    needToLink
			             || !io::file_exists( objectFile )
			             || ModifiedDate( objectFile ) >= outFileDate;
			
			objectFiles.push_back( objectFile );
		}
		
		std::string objectFilePaths = join( objectFiles.begin(),
		                                    objectFiles.end(),
		                                    " ",
		                                    std::ptr_fun( q ) );
		
		std::string link;
		
		std::string expeditedLib;
		
		if ( needLibs )
		{
			std::vector< std::string > usedLibFiles = GetUsedLibraries( project, targetName );
			
			// As long as needToLink is false, continue checking dates.
			// Stop as soon as we know we have to link (or we run out).
			if ( !needToLink )
			{
				std::vector< std::string >::const_iterator found;
				
				found = std::find_if( usedLibFiles.begin(),
				                      usedLibFiles.end(),
				                      more::compose1( std::bind2nd( std::not2( std::less< time_t >() ),
				                                                    outFileDate ),
				                                      more::ptr_fun( ModifiedDate ) ) );
				
				needToLink = found != usedLibFiles.end();
			}
			
			if ( !gnu )
			{
				for ( std::vector< std::string >::iterator it = usedLibFiles.begin();  it != usedLibFiles.end();  ++it )
				{
					if ( io::get_filename_string( *it ) == "Orion.lib" )
					{
						expeditedLib = q( *it );
						
						std::copy( it + 1, usedLibFiles.end(), it );
						
						usedLibFiles.resize( usedLibFiles.size() - 1 );
						
						break;
					}
				}
			}
			
			// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
			// Somehow, this is actually required to actually link anything with Mach-O.
			link << join( usedLibFiles.rbegin(),
			              usedLibFiles.rend(),
			              " ",
			              std::ptr_fun( q ) );
			
			// FIXME:  This is a hack
			if ( !gnu )
			{
				link << GetImports( project );
			}
		}
		
		if ( !needToLink )  return;
		
		
		if ( !gnu && project.CreatorCode().size() > 0 )
		{
			command << cmdgen.OutputCreator( project.CreatorCode() );
		}
		
		
		if ( needLibs && machO )
		{
			link << GetFrameworks( project );
		}
		
		if ( bundle )
		{
			std::string bundleName = linkName + ".app";
			
			CreateAppBundle( outputDir, bundleName );
			
			std::string contents( outputDir / bundleName / "Contents" );
			
			outFile = contents / "MacOS" / linkName;
			
			std::string pkgInfo = contents / "PkgInfo";
			
			try
			{
				//const N::OSType codeZero = N::OSType( 0 );
				
				//N::FSpCreate( pkgInfo, codeZero, codeZero );
				p7::open( pkgInfo, p7::o_creat, 0644 );
			}
			catch ( ... )  {}
			
			std::string info = "APPL" + project.CreatorCode();
			
			io::spray_file< NN::StringFlattener< std::string > >( pkgInfo, info );
		}
		
		if ( !useAr )
		{
			command << "-o";
		}
		
		command << q( outFile );
		
		command << expeditedLib << objectFilePaths << link;
		
		if ( gnu )
		{
			command << "> /tmp/link-errs.txt 2>&1";
		}
		else
		{
			command << " 2>&1 | filter-mwlink-warnings";
		}
		
		QueueCommand( "echo Linking:  " + io::get_filename_string( outFile ) );
		QueueCommand( command );
		
		std::string rsrcFile = outFile;
		
		if ( !project.UsedRezFiles().empty() )
		{
			const std::vector< FileName >& rez = project.UsedRezFiles();
			std::vector< std::string > rezFiles( rez.size() );
			
			std::transform( rez.begin(),
			                rez.end(),
			                rezFiles.begin(),
			                std::ptr_fun( RezLocation ) );
			
			if ( needCarbResource )
			{
				rezFiles.push_back( RezLocation( "CarbonApp.r" ) );
			}
			
			std::string includeDir = ProjectIncludesPath( project.ProjectFolder() );
			std::string rezCommand = "mpwrez -append";
			
			if ( gnu )
			{
				std::string bundleName = linkName + ".app";
				std::string rsrcFileName = linkName + ".rsrc";
				
				rsrcFile = outputDir / bundleName / "Contents" / "Resources" / rsrcFileName;
				
				rezCommand = "/Developer/Tools/Rez -append -i /Developer/Headers/FlatCarbon -useDF";
				
			}
			
			rezCommand << "-i " + q( includeDir );
			rezCommand << cmdgen.Output( rsrcFile );
			
			rezCommand << join( rezFiles.begin(),
			                    rezFiles.end(),
			                    " ",
			                    std::ptr_fun( q ) );
			
			QueueCommand( "echo Rezzing:  " + io::get_filename_string( rsrcFile ) );
			QueueCommand( rezCommand );
		}
		
		if ( !project.UsedRsrcFiles().empty() )
		{
			const std::vector< FileName >& rsrcs = project.UsedRsrcFiles();
			std::vector< std::string > rsrcFiles( rsrcs.size() );
			
			std::transform( rsrcs.begin(), 
			                rsrcs.end(),
			                rsrcFiles.begin(),
			                std::ptr_fun( RezLocation ) );
			
			std::string cpresCommand = "cpres";
			
			cpresCommand << join( rsrcFiles.begin(),
			                      rsrcFiles.end(),
			                      " ",
			                      std::ptr_fun( q ) );
			
			
			if ( gnu )
			{
				std::string echoes = join( rsrcFiles.begin(),
				                           rsrcFiles.end(),
				                           "; ",
				                           std::ptr_fun( MakeEchoedIncludedPOSIXPathname ) );
				
				cpresCommand = paren( echoes ) + " | /Developer/Tools/Rez -append -useDF -o";
			}
			
			cpresCommand << q( rsrcFile );
			
			QueueCommand( "echo Copying resources:  " + io::get_filename_string( rsrcFile ) );
			QueueCommand( cpresCommand );
		}
	}
	
}

