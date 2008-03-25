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
#include "io/slurp.hh"
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
	
	static void WritePkgInfo( const std::string& pathname, const std::string& contents )
	{
		p7::open( pathname, p7::o_creat, 0644 );
		
		if ( io::slurp_file< NN::StringFlattener< std::string > >( pathname ) != contents )
		{
			io::spew_file< NN::StringFlattener< std::string > >( pathname, contents );
		}
	}
	
	static bool ProjectLibsAreOutOfDate( const std::vector< ProjName >& usedProjects, const time_t& outFileDate )
	{
		std::vector< std::string >::const_iterator found;
		
		found = std::find_if( usedProjects.begin(),
		                      usedProjects.end(),
		                      more::compose1( std::bind2nd( std::not2( std::less< time_t >() ),
		                                                    outFileDate ),
		                                      more::compose1( more::ptr_fun( ModifiedDate ),
		                                                      more::ptr_fun( GetPathnameOfBuiltLibrary ) ) ) );
		
		bool outOfDate = found != usedProjects.end();
		
		return outOfDate;
	}
	
	static std::string GetLibraryLinkArgs( const std::vector< ProjName >& usedProjects, const time_t& outFileDate )
	{
		const bool outOfDate = outFileDate == 0;
		
		// If we already know we're out of date, don't bother checking further.
		// If we're fully up to date, return the null string.
		// It's up to the caller to avoid calling us if usedProjects is empty.
		
		if ( !outOfDate && !ProjectLibsAreOutOfDate( usedProjects, outFileDate ) )
		{
			return "";
		}
		
		// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
		// Somehow, this is actually required to actually link anything with Mach-O.
		return join( usedProjects.rbegin(),
		             usedProjects.rend(),
		             " ",
		             std::ptr_fun( GetLibraryLinkOption ) );
	}
	
	static void LinkFile( std::string         command,
	                      const std::string&  objectFileArgs,
	                      const std::string&  libraryArgs,
	                      const std::string&  trailer )
	{
		command << objectFileArgs << libraryArgs << trailer;
		
		QueueCommand( command );
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
	
	static bool FilesAreNewer( const std::vector< std::string >& files, const time_t& date )
	{
		std::vector< std::string >::const_iterator it, end = files.end();
		
		for ( it = files.begin();  it != end;  ++it )
		{
			const std::string& pathname = *it;
			
			if ( !io::file_exists( pathname ) || ModifiedDate( pathname ) >= date )
			{
				return true;
			}
		}
		
		return false;
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
		
		bool hasStaticLib = false;
		bool hasExecutable = true;
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
				hasStaticLib = true;
				hasExecutable = false;
				break;
			
			case productToolkit:
				command = cmdgen.LibraryMakerName();
				linkName = gLibraryPrefix + project.Name() + gLibraryExtension;
				gccSupported = true;
				hasStaticLib = true;
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
		
		std::string linkDir = outputDir;
		
		if ( bundle )
		{
			std::string bundleName = linkName + ".app";
			
			CreateAppBundle( outputDir, bundleName );
			
			std::string contents( outputDir / bundleName / "Contents" );
			
			linkDir = contents / "MacOS";
			
			WritePkgInfo( contents / "PkgInfo", "APPL" + project.CreatorCode() );
		}
		
		std::string outFile = linkDir / linkName;
		
		bool outFileExists = io::item_exists( outFile );
		
		time_t outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = Options().all || !outFileExists;
		
		std::vector< std::string > objectFiles( project.Sources().size() );
		
		std::transform( project.Sources().begin(),
		                project.Sources().end(),
		                objectFiles.begin(),
		                more::compose1( std::bind1st( more::ptr_fun( static_cast< std::string (*)( const std::string&, const std::string& ) >( operator/ ) ),
		                                              objectsDir ),
		                                more::compose1( more::ptr_fun( ObjectFileName ),
		                                                more::ptr_fun( static_cast< std::string (*)( const std::string& ) >( io::get_filename ) ) ) ) );
		
		needToLink = needToLink || FilesAreNewer( objectFiles, outFileDate );
		
		std::string objectFilePaths = join( objectFiles.begin(),
		                                    objectFiles.end(),
		                                    " ",
		                                    std::ptr_fun( q ) );
		
		std::string trailer = gnu ? "> /tmp/link-errs.txt 2>&1"
		                          : " 2>&1 | filter-mwlink-warnings";
		
		if ( hasStaticLib && needToLink )
		{
			std::string link = command;
			
			const bool useAr = gnu;
			
			if ( !useAr )
			{
				link << cmdgen.LinkerOptions();
				link << "-o";
			}
			
			link << q( outFile );
			
			QueueCommand( "echo Linking:  " + io::get_filename_string( outFile ) );
			
			LinkFile( link, objectFilePaths, "", trailer );
		}
		
		if ( !needLibs )
		{
			return;
		}
		
		std::string allLibraryLinkArgs;
		
		std::vector< ProjName > usedProjects = project.AllUsedProjects();
		
		usedProjects.pop_back();  // we're last; drop us
		
		RemoveNonLibs( usedProjects );
		
		if ( !usedProjects.empty() )
		{
			allLibraryLinkArgs = "-L'" + libsDir + "'";
			
			std::string projLibLinkArgs = GetLibraryLinkArgs( usedProjects, needToLink ? 0 : outFileDate );
			
			if ( projLibLinkArgs.empty() )
			{
				return;
			}
			
			allLibraryLinkArgs << projLibLinkArgs;
		}
		
		// FIXME:  This is a hack
		if ( !gnu )
		{
			allLibraryLinkArgs << GetImports( project );
		}
		
		if ( machO )
		{
			allLibraryLinkArgs << GetFrameworks( project );
		}
		
		
		if ( !gnu && project.CreatorCode().size() > 0 )
		{
			command << cmdgen.OutputCreator( project.CreatorCode() );
		}
		
		const bool useAr = gnu  &&  hasStaticLib;
		
		if ( !useAr )
		{
			command << cmdgen.LinkerOptions();
			command << "-o";
		}
		
		command << q( outFile );
		
		QueueCommand( "echo Linking:  " + io::get_filename_string( outFile ) );
		
		LinkFile( command, objectFilePaths, allLibraryLinkArgs, trailer );
		
		std::string rsrcFile = gnu ? outputDir / BundleResourceFileRelativePath( linkName )
		                           : outFile;
		
		bool usingOSXRez = gnu;
		
		CompileResources( project, rsrcFile, needCarbResource, usingOSXRez );
		
		CopyResources( project, rsrcFile, usingOSXRez );
	}
	
}

