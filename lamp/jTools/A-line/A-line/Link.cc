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
#include "A-line/ProjectCommon.hh"


namespace ALine
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	
	// "path/to/file" -> "path/to/file"
	// "path/to/-lib" -> "-wi path/to/lib"
	
	static Command MakeImport( const std::string& file )
	{
		Command result;
		
		const char* importPath = file.c_str();
		
		if ( importPath[ 0 ] == '-' )
		{
			++importPath;
			
			result.push_back( "-wi" );
		}
		
		result.push_back( importPath );
		
		return result;
	}
	
	
	// "path/to/project", "library" -> "path/to/project/library" | "library"
	
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
	
	static Command GetImports( const std::vector< std::string >& importFiles )
	{
		Command imports;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = importFiles.begin();  it != importFiles.end();  ++it )
		{
			AugmentCommand( imports, MakeImport( *it ) );
		}
		
		return imports;
	}
	
	
	static Command MakeFramework( const char* framework )
	{
		return MakeCommand( "-framework", framework );
	}
	
	static Command GetFrameworks( const Project& project )
	{
		const std::vector< FileName >& frameworkNames( project.Frameworks() );
		
		if ( frameworkNames.size() == 0 )
		{
			return MakeFramework( "Carbon" );
		}
		
		Command frameworks;
		
		typedef std::vector< FileName >::const_iterator Iter;
		
		for ( Iter it = frameworkNames.begin();  it != frameworkNames.end();  ++it )
		{
			AugmentCommand( frameworks, MakeFramework( it->c_str() ) );
		}
		
		return frameworks;
	}
	
	template < class Iter >
	static bool FilesAreNewer( Iter begin, Iter end, const time_t& date )
	{
		for ( Iter it = begin;  it != end;  ++it )
		{
			const std::string& pathname = *it;
			
			if ( !io::file_exists( pathname ) || ModifiedDate( pathname ) >= date )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static bool SourceFileIsTool( const std::string& sourceFile, const std::vector< std::string >& tools )
	{
		return std::find( tools.begin(),
		                  tools.end(),
		                  io::get_filename( sourceFile ) ) != tools.end();
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
	
	static std::string gLibraryPrefix;
	static std::string gLibraryExtension;
	
	static std::string GetPathnameOfBuiltLibrary( const std::string& name )
	{
		return LibrariesDirPath() / gLibraryPrefix + name + gLibraryExtension;
	}
	
	static bool ProjectLibsAreOutOfDate( const std::vector< ProjName >& usedProjects, const time_t& outFileDate )
	{
		std::vector< std::string >::const_iterator found;
		
		// Search for another project's library that's newer than our link output.
		found = std::find_if( usedProjects.begin(),
		                      usedProjects.end(),
		                      more::compose1( std::bind2nd( std::not2( std::less< time_t >() ),
		                                                    outFileDate ),
		                                      more::compose1( more::ptr_fun( ModifiedDate ),
		                                                      more::ptr_fun( GetPathnameOfBuiltLibrary ) ) ) );
		
		bool outOfDate = found != usedProjects.end();
		
		return outOfDate;
	}
	
	static Command MakeLibraryLinkArgs( std::vector< ProjName >& usedLibs, const time_t& outFileDate )
	{
		Command result;
		
		const bool outOfDate = outFileDate == 0;
		
		// If we're fully up to date, return an empty container.
		// It's up to the caller to avoid calling us if usedLibs is empty.
		
		if ( !outOfDate && !ProjectLibsAreOutOfDate( usedLibs, outFileDate ) )
		{
			return result;
		}
		
		// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
		// Somehow, this is actually required to actually link anything on Unix.
		
		typedef std::vector< ProjName >::reverse_iterator Iter;
		
		for ( Iter it = usedLibs.rbegin();  it != usedLibs.rend();  ++it )
		{
			ProjName& name = *it;
			
			name = "-l" + name;
			
			result.push_back( name.c_str() );
		}
		
		return result;
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		return filename + ".txt";
	}
	
	static void LinkFile( Command                            command,
	                      const std::string&                 outputFile,
	                      const std::vector< std::string >&  objectFileArgs,
	                      const Command                   &  libraryArgs,
	                      const std::string&                 diagnosticsDir )
	{
		command.push_back( outputFile.c_str() );
		
		AugmentCommand( command, objectFileArgs );
		
		AugmentCommand( command, libraryArgs );
		
		std::string filename = io::get_filename_string( outputFile );
		
		command.push_back( NULL );
		
		std::string diagnosticsFile = diagnosticsDir / DiagnosticsFilenameFromSourceFilename( filename );
		
		TaskPtr link( new CommandTask( command, diagnosticsFile, "Linking: " + filename ) );
		
		link->Main();
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
		
		Command rezCommand;
		
		if ( usingOSXRez )
		{
			rezCommand.push_back( "/Developer/Tools/Rez" );
			rezCommand.push_back( "-i" );
			rezCommand.push_back( "/Developer/Headers/FlatCarbon" );
			rezCommand.push_back( "-useDF" );
		}
		else
		{
			rezCommand.push_back( "mpwrez" );
		}
		
		rezCommand.push_back( "-append" );
		
		std::string includeDir = ProjectIncludesPath( project.ProjectFolder() );
		
		rezCommand.push_back( "-i" );
		
		rezCommand.push_back( includeDir.c_str() );
		
		AugmentCommand( rezCommand, OutputOption( rsrcFile.c_str() ) );
		
		AugmentCommand( rezCommand, rezFiles );
		
		rezCommand.push_back( NULL );
		
		TaskPtr rezTask( new CommandTask( rezCommand, "", "Rezzing: " + io::get_filename_string( rsrcFile ) ) );
		
		rezTask->Main();
	}
	
	// foo.r -> echo 'include "foo.r";'
	static std::string MakeEchoedRezInclude( const std::string& file )
	{
		std::string include = "include ";
		std::string echo    = "echo ";
		
		return echo + q( include + qq( file ) + ";" );
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
		
		Command command;
		
		std::string command_line;
		
		if ( usingOSXRez )
		{
			command_line = join( rsrcFiles.begin(),
			                     rsrcFiles.end(),
			                     "; ",
			                     std::ptr_fun( MakeEchoedRezInclude ) );
			
			command_line = paren( command_line );
			
			command_line += " | /Developer/Tools/Rez -append -useDF -o ";
			command_line += q( rsrcFile );
			
			command.push_back( "/bin/sh" );
			command.push_back( "-c" );
			command.push_back( command_line.c_str() );
		}
		else
		{
			command.push_back( "cpres" );
			
			AugmentCommand( command, rsrcFiles );
			
			command.push_back( rsrcFile.c_str() );
		}
		
		command.push_back( NULL );
		
		TaskPtr cpres( new CommandTask( command, "", "Copying resources: " + io::get_filename_string( rsrcFile ) ) );
		
		cpres->Main();
	}
	
	static time_t EffectiveModifiedDate( const std::string& file )
	{
		return Options().all || !io::item_exists( file ) ? 0 : ModifiedDate( file );
	}
	
	void LinkProduct( const Project& project, TargetInfo targetInfo )
	{
		const bool gnu = targetInfo.toolkit == toolkitGNU;
		
		gLibraryPrefix    = gnu ? "lib" : "";
		gLibraryExtension = gnu ? ".a" : ".lib";
		
		const bool machO = targetInfo.platform & CD::runtimeMachO;
		
		std::string diagnosticsDir = ProjectDiagnosticsDirPath( project.Name() );
		
		CommandGenerator cmdgen( targetInfo );
		
		std::string linkName = project.ProgramName();
		
		if ( linkName == "" )
		{
			linkName = project.Name();
		}
		
		Command command;
		
		command.push_back( cmdgen.LinkerName() );
		
		AugmentCommand( command, cmdgen.TargetArchitecture() );
		
		bool hasStaticLib = false;
		bool hasExecutable = true;
		bool needCarbResource = false;
		bool gccSupported = false;
		bool bundle = false;
		bool toolkit = false;
		
		const CD::Platform carbonCFM = CD::apiMacCarbon | CD::runtimeCodeFragments;
		
		std::string driverName;
		
		switch ( project.Product() )
		{
			case productStaticLib:
				linkName = gLibraryPrefix + project.Name() + gLibraryExtension;
				gccSupported = true;
				hasStaticLib = true;
				hasExecutable = false;
				break;
			
			case productToolkit:
				if ( cmdgen.TargetCommandLineTool()[0] )
				{
					command.push_back( cmdgen.TargetCommandLineTool() );
				}
				
				linkName = gLibraryPrefix + project.Name() + gLibraryExtension;
				gccSupported = true;
				hasStaticLib = true;
				toolkit = true;
				break;
			
			case productApplication:
				if ( cmdgen.TargetApplication()[0] )
				{
					command.push_back( cmdgen.TargetApplication() );
				}
				
				needCarbResource = (targetInfo.platform & carbonCFM) == carbonCFM;
				gccSupported = true;
				bundle = gnu;
				break;
			
			case productINIT:
				command.push_back( cmdgen.MWTargetCodeResource() );
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "INIT=0" ) );
				AugmentCommand( command, cmdgen.OutputType       ( "INIT"   ) );
				break;
			
			case productDriver:
				driverName = "." + project.Name();
				
				command.push_back( cmdgen.MWTargetCodeResource() );
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "DRVR=0" ) );
				AugmentCommand( command, cmdgen.ResourceName     ( driverName.c_str() ) );
				AugmentCommand( command, cmdgen.OutputType       ( "INIT"   ) );
				AugmentCommand( command, cmdgen.OutputCreator    ( "RSED"   ) );
				
				command.push_back( cmdgen.CustomDriverHeader() );
				break;
			
			case productTool:
				if ( cmdgen.TargetCommandLineTool()[0] )
				{
					command.push_back( cmdgen.TargetCommandLineTool() );
				}
				
				gccSupported = true;
				break;
			
			case productSharedLib:
				command.push_back( cmdgen.MWTargetSharedLibrary() );
				break;
		}
		
		if ( gnu  &&  !gccSupported )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Sorry, GCC is not supported for this type of product.\n" ) );
			
			p7::throw_errno( EINVAL );
		}
		
		std::string objectsDir = ProjectObjectsDirPath( project.Name() );
		
		std::string libsDir = LibrariesDirPath();
		
		std::string outputDir = ProjectOutputDirPath( project.Name() );
		
		std::string linkDir = hasStaticLib ? libsDir : outputDir;
		
		if ( bundle )
		{
			std::string bundleName = linkName + ".app";
			
			CreateAppBundle( outputDir, bundleName );
			
			std::string contents( outputDir / bundleName / "Contents" );
			
			linkDir = contents / "MacOS";
			
			WritePkgInfo( contents / "PkgInfo", "APPL" + project.CreatorCode() );
		}
		
		std::string outFile = linkDir / linkName;
		
		std::vector< std::string > toolSourceFiles = project.ToolSourceFiles();
		
		std::sort( toolSourceFiles.begin(), toolSourceFiles.end() );
		
		std::vector< std::string > sourceFiles = project.Sources();
		
		std::size_t n_tools = toolkit ? std::partition( sourceFiles.begin(),
		                                                sourceFiles.end(),
		                                                std::bind2nd( more::ptr_fun( &SourceFileIsTool ),
		                                                                              toolSourceFiles ) ) - sourceFiles.begin()
		                              : 0;
		
		std::vector< std::string > objectFiles( sourceFiles.size() );
		
		std::transform( sourceFiles.begin(),
		                sourceFiles.end(),
		                objectFiles.begin(),
		                more::compose1( std::bind1st( more::ptr_fun( static_cast< std::string (*)( const std::string&, const std::string& ) >( operator/ ) ),
		                                              objectsDir ),
		                                more::compose1( more::ptr_fun( ObjectFileName ),
		                                                more::ptr_fun( static_cast< std::string (*)( const std::string& ) >( io::get_filename ) ) ) ) );
		
		std::vector< std::string > objectFilePaths( objectFiles.begin() + n_tools,
		                                            objectFiles.end() );
		
		std::string trailer = gnu ? "> /tmp/link-errs.txt 2>&1"
		                          : " 2>&1 | filter-mwlink-warnings";
		
		time_t outFileDate = EffectiveModifiedDate( outFile );
		
		if ( FilesAreNewer( objectFiles.begin() + n_tools, objectFiles.end(), outFileDate ) )
		{
			outFileDate = 0;
		}
		
		if ( hasStaticLib  &&  outFileDate == 0 )
		{
			Command link = cmdgen.LibraryMakerName();
			
			const bool useAr = gnu;
			
			if ( !useAr )
			{
				if ( cmdgen.LinkerOptions()[0] )
				{
					link.push_back( cmdgen.LinkerOptions() );
				}
				
				link.push_back( "-o" );
			}
			
			LinkFile( link, outFile, objectFilePaths, Command(), diagnosticsDir );
		}
		
		if ( !hasExecutable )
		{
			return;
		}
		
		Command allLibraryLinkArgs;
		
		// A copy so we can munge it
		std::vector< ProjName > usedProjects = project.AllUsedProjects();
		
		usedProjects.pop_back();  // we're last; drop us
		
		RemoveNonLibs( usedProjects );
		
		std::string libsDirOption;
		
		if ( !usedProjects.empty() )
		{
			libsDirOption = "-L" + libsDir;
			
			allLibraryLinkArgs.push_back( libsDirOption.c_str() );
			
			Command projLibLinkArgs = MakeLibraryLinkArgs( usedProjects, outFileDate );
			
			if ( projLibLinkArgs.empty() )
			{
				return;
			}
			
			
			
			AugmentCommand( allLibraryLinkArgs, projLibLinkArgs );
		}
		
		std::vector< std::string > allImports;
		
		// FIXME:  This is a hack
		if ( !gnu )
		{
			allImports = GetAllImports( project );
			
			AugmentCommand( allLibraryLinkArgs, GetImports( allImports ) );
		}
		
		if ( machO )
		{
			AugmentCommand( allLibraryLinkArgs, GetFrameworks( project ) );
		}
		
		
		if ( !gnu && project.CreatorCode().size() > 0 )
		{
			AugmentCommand( command, cmdgen.OutputCreator( project.CreatorCode().c_str() ) );
		}
		
		if ( cmdgen.LinkerOptions()[0] )
		{
			command.push_back( cmdgen.LinkerOptions() );
		}
		
		command.push_back( "-o" );
		
		if ( toolkit )
		{
			allLibraryLinkArgs.insert( allLibraryLinkArgs.begin(), outFile.c_str() );
			
			typedef std::vector< std::string >::const_iterator Iter;
			
			const Iter end = objectFiles.begin() + n_tools;
			
			for ( Iter it = objectFiles.begin();  it != end;  ++it )
			{
				const std::string& objectFile = *it;
				
				std::string linkOutput = outputDir / io::get_filename( objectFile );
				
				linkOutput.resize( linkOutput.size() - 2 );  // truncate ".o"
				
				if ( EffectiveModifiedDate( objectFile ) >= EffectiveModifiedDate( linkOutput ) )
				{
					std::vector< std::string > objectFilePaths( 1, objectFile );
					
					LinkFile( command, linkOutput, objectFilePaths, allLibraryLinkArgs, diagnosticsDir );
				}
			}
		}
		else
		{
			LinkFile( command, outFile, objectFilePaths, allLibraryLinkArgs, diagnosticsDir );
			
			std::string rsrcFile = gnu ? outputDir / BundleResourceFileRelativePath( linkName )
			                           : outFile;
			
			bool usingOSXRez = gnu;
			
			CompileResources( project, rsrcFile, needCarbResource, usingOSXRez );
			
			CopyResources( project, rsrcFile, usingOSXRez );
		}
	}
	
}

