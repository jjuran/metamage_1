/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Iota
#include "iota/strings.hh"

// Io
#include "io/io.hh"
#include "io/spew.hh"

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
	
	// foo.r -> echo 'include "foo.r";'
	static std::string MakeEchoedRezInclude( const std::string& file )
	{
		std::string include = "include ";
		std::string echo    = "echo ";
		
		return echo + q( include + qq( file ) + ";" );
	}
	
	static std::string MakeFramework( const std::string& framework )
	{
		return std::string( "-framework " ) + framework;
	}
	
	
	static std::string MakeLibraryPathname( const std::string& projectFolder, const std::string& libName )
	{
		if ( libName[0] == '-' )
		{
			return libName;
		}
		
		std::string pathname = projectFolder / libName;
		
		return io::file_exists( pathname ) ? pathname : libName;
	}
	
	static void CopyImports( const ProjName& projName,
	                         std::back_insert_iterator< std::vector< std::string > > inserter )
	{
		Project& project = GetProject( projName );
		
		std::vector< FileName > importNames( project.LibImports() );
		
		std::transform( importNames.begin(),
		                importNames.end(),
		                inserter,
		                std::bind1st( more::ptr_fun( MakeLibraryPathname ),
		                              project.ProjectFolder() ) );
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
		
		return imports;
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
	
	static std::string GetLibraryLinkOption( const std::string& name )
	{
		return "-l" + name;
	}
	
	static std::string gLibraryPrefix;
	static std::string gLibraryExtension;
	
	static std::string GetPathnameOfBuiltLibrary( const std::string& name )
	{
		return LibrariesDirPath() / gLibraryPrefix + name + gLibraryExtension;
	}
	
	static bool ProjectBuildsLib( const Project& project )
	{
		return project.Product() == productStaticLib;
	}
	
	static void RemoveNonLibs( std::vector< ProjName >& usedProjects )
	{
		usedProjects.resize( std::remove_if( usedProjects.begin(),
		                                     usedProjects.end(),
		                                     more::compose1( std::not1( more::ptr_fun( ProjectBuildsLib ) ),
		                                                     more::ptr_fun( GetProject ) ) ) - usedProjects.begin() );
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
	
	static std::string BundleResourceFileRelativePath( const std::string& linkName )
	{
		std::string bundleName   = linkName + ".app";
		std::string rsrcFileName = linkName + ".rsrc";
		
		return bundleName / "Contents" / "Resources" / rsrcFileName;
	}
	
	static void CompileResources( const Project&      project,
	                              const std::string&  rsrcFile,
	                              bool                needsCarbResource,
	                              bool                usingOSXRez )
	{
		const std::vector< FileName >& rez = project.UsedRezFiles();
		
		if ( rez.empty() )
		{
			return;
		}
		
		std::vector< std::string > rezFiles( rez.size() );
		
		std::transform( rez.begin(),
		                rez.end(),
		                rezFiles.begin(),
		                std::ptr_fun( RezLocation ) );
		
		if ( needsCarbResource )
		{
			rezFiles.push_back( RezLocation( "CarbonApp.r" ) );
		}
		
		std::string rezCommand = usingOSXRez ? "/Developer/Tools/Rez -append -i /Developer/Headers/FlatCarbon -useDF"
		                                     : "mpwrez -append";
		
		std::string includeDir = ProjectIncludesPath( project.ProjectFolder() );
		
		rezCommand << "-i " + q( includeDir );
		rezCommand << OutputOption( rsrcFile );
		
		rezCommand << join( rezFiles.begin(),
		                    rezFiles.end(),
		                    " ",
		                    std::ptr_fun( q ) );
		
		QueueCommand( "echo Rezzing:  " + io::get_filename_string( rsrcFile ) );
		QueueCommand( rezCommand );
	}
	
	static void CopyResources( const Project&      project,
	                           const std::string&  rsrcFile,
	                           bool                usingOSXRez )
	{
		const std::vector< FileName >& rsrcs = project.UsedRsrcFiles();
		
		if ( rsrcs.empty() )
		{
			return;
		}
		
		std::vector< std::string > rsrcFiles( rsrcs.size() );
		
		std::transform( rsrcs.begin(), 
		                rsrcs.end(),
		                rsrcFiles.begin(),
		                std::ptr_fun( RezLocation ) );
		
		std::string command_line = join( rsrcFiles.begin(),
		                                 rsrcFiles.end(),
		                                 usingOSXRez ? "; "
		                                             : " ",
		                                 usingOSXRez ? std::ptr_fun( MakeEchoedRezInclude )
		                                             : std::ptr_fun( q ) );
		
		
		if ( usingOSXRez )
		{
			command_line = paren( command_line ) + " | /Developer/Tools/Rez -append -useDF -o";
		}
		else
		{
			command_line = "cpres " + command_line;
		}
		
		command_line << q( rsrcFile );
		
		QueueCommand( "echo Copying resources:  " + io::get_filename_string( rsrcFile ) );
		QueueCommand( command_line );
	}
	
	void LinkProduct( const Project& project, TargetInfo targetInfo )
	{
		const bool gnu = targetInfo.toolkit == toolkitGNU;
		
		gLibraryPrefix    = gnu ? "lib" : "";
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
				linkName = gLibraryPrefix + project.Name() + gLibraryExtension;
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
			
			case productTool:
				gccSupported = true;
				command << cmdgen.TargetCommandLineTool();
				break;
			
			case productSharedLib:
				command << cmdgen.MWTargetSharedLibrary();
				break;
		}
		
		if ( gnu  &&  !gccSupported )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Sorry, GCC is not supported for this type of product.\n" ) );
			
			p7::throw_errno( EINVAL );
		}
		
		std::string objectsDir = ProjectObjectsDirPath( project.Name() );
		
		std::string libsDir = LibrariesDirPath();
		
		std::string outputDir  = project.Product() == productStaticLib ? libsDir
		                                                               : ProjectOutputDirPath( project.Name() );
		
		std::string outFile = outputDir / linkName;
		bool outFileExists = io::item_exists( outFile );
		
		time_t outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = Options().all || !outFileExists;
		
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
		
		std::string libSearch;
		
		if ( needLibs )
		{
			libSearch = "-L'" + libsDir + "'";
			
			std::vector< ProjName > usedProjects = project.AllUsedProjects();
			
			usedProjects.pop_back();  // we're last; drop us
			
			RemoveNonLibs( usedProjects );
			
			// As long as needToLink is false, continue checking dates.
			// Stop as soon as we know we have to link (or we run out).
			if ( !needToLink )
			{
				std::vector< std::string >::const_iterator found;
				
				found = std::find_if( usedProjects.begin(),
				                      usedProjects.end(),
				                      more::compose1( std::bind2nd( std::not2( std::less< time_t >() ),
				                                                    outFileDate ),
				                                      more::compose1( more::ptr_fun( ModifiedDate ),
				                                                      more::ptr_fun( GetPathnameOfBuiltLibrary ) ) ) );
				
				needToLink = found != usedProjects.end();
			}
			
			if ( !gnu )
			{
				for ( std::vector< std::string >::iterator it = usedProjects.begin();  it != usedProjects.end();  ++it )
				{
					if ( *it == "Orion" )
					{
						expeditedLib = GetLibraryLinkOption( *it );
						
						std::copy( it + 1, usedProjects.end(), it );
						
						usedProjects.resize( usedProjects.size() - 1 );
						
						break;
					}
				}
			}
			
			// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
			// Somehow, this is actually required to actually link anything with Mach-O.
			link << join( usedProjects.rbegin(),
			              usedProjects.rend(),
			              " ",
			              std::ptr_fun( GetLibraryLinkOption ) );
			
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
			
			p7::open( pkgInfo, p7::o_creat, 0644 );
			
			std::string info = "APPL" + project.CreatorCode();
			
			io::spew_file< NN::StringFlattener< std::string > >( pkgInfo, info );
		}
		
		const bool useAr = gnu  &&  project.Product() == productStaticLib;
		
		if ( !useAr )
		{
			command << cmdgen.LinkerOptions();
			command << "-o";
		}
		
		command << q( outFile );
		
		command << libSearch << expeditedLib << objectFilePaths << link;
		
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
		
		std::string rsrcFile = gnu ? outputDir / BundleResourceFileRelativePath( linkName )
		                           : outFile;
		
		bool usingOSXRez = gnu;
		
		CompileResources( project, rsrcFile, needCarbResource, usingOSXRez );
		
		CopyResources( project, rsrcFile, usingOSXRez );
	}
	
}

