/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <numeric>
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
	
	static void AddImport( const std::string& file, std::vector< std::string >& v )
	{
		const char* importPath = file.c_str();
		
		if ( importPath[ 0 ] == '-' )
		{
			++importPath;
			
			v.push_back( "-wi" );
		}
		
		v.push_back( importPath );
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
	
	static void AddImports( const std::vector< std::string >& importFiles, std::vector< std::string >& v )
	{
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = importFiles.begin();  it != importFiles.end();  ++it )
		{
			AddImport( *it, v );
		}
	}
	
	
	static void AddFrameworks( const std::vector< std::string >& frameworkNames, std::vector< std::string >& v )
	{
		typedef std::vector< FileName >::const_iterator Iter;
		
		for ( Iter it = frameworkNames.begin();  it != frameworkNames.end();  ++it )
		{
			v.push_back( "-framework" );
			v.push_back( *it );
		}
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
	
	static void AddLibraryLinkArgs( const std::vector< ProjName >& usedLibs, std::vector< std::string >& v )
	{
		// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
		// Somehow, this is actually required to actually link anything on Unix.
		
		typedef std::vector< ProjName >::const_reverse_iterator Iter;
		
		for ( Iter it = usedLibs.rbegin();  it != usedLibs.rend();  ++it )
		{
			const ProjName& name = *it;
			
			v.push_back( "-l" + name );
		}
	}
	
	static std::string DiagnosticsFilenameFromSourceFilename( const std::string& filename )
	{
		return filename + ".txt";
	}
	
	static void UpdateInputStamp( const TaskPtr& task, const std::string& input_pathname )
	{
		task->UpdateInputStamp( ModifiedDate( input_pathname ) );
	}
	
	class LinkingTask : public Task
	{
		private:
			Command                     itsCommand;
			std::string                 itsOutputPathname;
			std::vector< std::string >  itsInputArguments;
			std::string                 itsDiagnosticsDir;
		
		public:
			LinkingTask( const Command&                     command,
			             const std::string&                 output,
			             const std::vector< std::string >&  input,
			             const std::string&                 diagnostics ) : itsCommand       ( command     ),
			                                                                itsOutputPathname( output      ),
			                                                                itsInputArguments( input       ),
			                                                                itsDiagnosticsDir( diagnostics )
			{
			}
			
			template < class Iter >
			LinkingTask( const Command&      command,
			             const std::string&  output,
			             Iter                input_begin,
			             Iter                input_end,
			             const std::string&  diagnostics ) : itsCommand       ( command     ),
			                                                 itsOutputPathname( output      ),
			                                                 itsInputArguments( input_begin, input_end ),
			                                                 itsDiagnosticsDir( diagnostics )
			{
			}
			
			void Main();
	};
	
	void LinkingTask::Main()
	{
		// If the output file exists and it's up to date, we can skip compiling.
		
		if ( io::item_exists( itsOutputPathname ) )
		{
			time_t output_stamp = ModifiedDate( itsOutputPathname );
			
			if ( UpToDate( output_stamp ) )
			{
				return;
			}
		}
		
		itsCommand.push_back( itsOutputPathname.c_str() );
		
		AugmentCommand( itsCommand, itsInputArguments );
		
		std::string output_filename = io::get_filename_string( itsOutputPathname );
		
		itsCommand.push_back( NULL );
		
		std::string diagnostics_pathname = itsDiagnosticsDir / DiagnosticsFilenameFromSourceFilename( output_filename );
		
		RunCommand( itsCommand, diagnostics_pathname.c_str(), "Linking: " + output_filename );
		
		UpdateInputStamp( ModifiedDate( itsOutputPathname ) );
	}
	
	static std::string BundleResourceFileRelativePath( const std::string& linkName )
	{
		std::string bundleName   = linkName + ".app";
		std::string rsrcFileName = linkName + ".rsrc";
		
		return bundleName / "Contents" / "Resources" / rsrcFileName;
	}
	
	class RezzingTask : public Task
	{
		private:
			std::vector< std::string >  itsInputPathnames;
			std::string                 itsOutputPathname;
			std::string                 itsIncludeDirPathname;
			bool                        itIsUsingOSX;
		
		public:
			RezzingTask( const std::vector< std::string >&  input,
			             const std::string&                 output,
			             const std::string&                 includeDir,
			             bool                               usingOSX ) : itsInputPathnames    ( input      ),
			                                                             itsOutputPathname    ( output     ),
			                                                             itsIncludeDirPathname( includeDir ),
			                                                             itIsUsingOSX         ( usingOSX   )
			{
			}
			
			void Main();
	};
	
	void RezzingTask::Main()
	{
		if ( io::item_exists( itsOutputPathname ) )
		{
			time_t output_stamp = ModifiedDate( itsOutputPathname );
			
			if ( UpToDate( output_stamp ) )
			{
				return;
			}
		}
		
		Command rezCommand;
		
		if ( itIsUsingOSX )
		{
			rezCommand.push_back( "/Developer/Tools/Rez" );
			rezCommand.push_back( "-i" );
			rezCommand.push_back( "/Developer/Headers/FlatCarbon" );
			rezCommand.push_back( "-useDF" );
		}
		else
		{
			rezCommand.push_back( "mpwrez" );
			rezCommand.push_back( "-c" );
			rezCommand.push_back( "RSED" );
			rezCommand.push_back( "-t" );
			rezCommand.push_back( "rsrc" );
		}
		
		rezCommand.push_back( "-i" );
		
		rezCommand.push_back( itsIncludeDirPathname.c_str() );
		
		AugmentCommand( rezCommand, OutputOption( itsOutputPathname.c_str() ) );
		
		AugmentCommand( rezCommand, itsInputPathnames );
		
		rezCommand.push_back( NULL );
		
		RunCommand( rezCommand, NULL, "Rezzing: " + io::get_filename_string( itsOutputPathname ) );
		
		UpdateInputStamp( ModifiedDate( itsOutputPathname ) );
	}
	
	static TaskPtr MakeRezTask( const Project&      project,
	                            const std::string&  output_pathname,
	                            bool                needsCarbResource,
	                            bool                usingOSXRez )
	{
		const std::vector< FileName >& input_filenames = project.UsedRezFiles();
		
		std::vector< std::string > input_pathnames( input_filenames.size() );
		
		std::transform( input_filenames.begin(),
		                input_filenames.end(),
		                input_pathnames.begin(),
		                std::ptr_fun( RezLocation ) );
		
		if ( needsCarbResource )
		{
			input_pathnames.push_back( RezLocation( "CarbonApp.r" ) );
		}
		
		std::string includeDir = ProjectIncludesPath( project.ProjectFolder() );
		
		TaskPtr rez_task( new RezzingTask( input_pathnames, output_pathname, includeDir, usingOSXRez ) );
		
		std::for_each( input_pathnames.begin(),
		               input_pathnames.end(),
		               std::bind1st( more::ptr_fun( UpdateInputStamp ), rez_task ) );
		
		return rez_task;
	}
	
	// foo.r -> echo -n 'include "foo.r";'
	static std::string MakeEchoedRezInclude( const std::string& file )
	{
		std::string include = "include ";
		std::string echo    = "echo -n ";  // OS X Rez can't handle Unix newlines
		
		return echo + q( include + qq( file ) + ";" );
	}
	
	class ResourceCopyingTask : public Task
	{
		private:
			std::vector< std::string >  itsInputPathnames;
			std::string                 itsOutputPathname;
			bool                        itIsUsingOSX;
		
		public:
			ResourceCopyingTask( const std::vector< std::string >&  input,
			                     const std::string&                 output,
			                     bool                               usingOSX ) : itsInputPathnames( input    ),
			                                                                     itsOutputPathname( output   ),
			                                                                     itIsUsingOSX     ( usingOSX )
			{
			}
			
			void Main();
	};
	
	void ResourceCopyingTask::Main()
	{
		if ( io::item_exists( itsOutputPathname ) )
		{
			time_t output_stamp = ModifiedDate( itsOutputPathname );
			
			if ( UpToDate( output_stamp ) )
			{
				return;
			}
		}
		
		Command command;
		
		std::string command_line;
		
		if ( itIsUsingOSX )
		{
			command_line = join( itsInputPathnames.begin(),
			                     itsInputPathnames.end(),
			                     "; ",
			                     std::ptr_fun( MakeEchoedRezInclude ) );
			
			command_line = paren( command_line );
			
			command_line += " | /Developer/Tools/Rez -append -useDF -o ";
			command_line += q( itsOutputPathname );
			
			command.push_back( "/bin/sh" );
			command.push_back( "-c" );
			command.push_back( command_line.c_str() );
		}
		else
		{
			command.push_back( "cpres" );
			
			AugmentCommand( command, itsInputPathnames );
			
			command.push_back( itsOutputPathname.c_str() );
		}
		
		command.push_back( NULL );
		
		RunCommand( command, "", "Copying resources: " + io::get_filename_string( itsOutputPathname ) );
		
		UpdateInputStamp( ModifiedDate( itsOutputPathname ) );
	}
	
	static time_t EffectiveModifiedDate( const std::string& file )
	{
		return Options().all || !io::item_exists( file ) ? 0 : ModifiedDate( file );
	}
	
	inline time_t later_of_time_or_library_mod_stamp( time_t a, const std::string& b )
	{
		return std::max( a, ModifiedDate( GetPathnameOfBuiltLibrary( b ) ) );
	}
	
	static time_t LatestLibraryModificationDate( const std::vector< ProjName >& used_project_names )
	{
		// Get the time of the latest modification to any built library we use.
		return std::accumulate( used_project_names.begin(),
		                        used_project_names.end(),
		                        0,
		                        later_of_time_or_library_mod_stamp );
	}
	
	template < class Iter >
	static TaskPtr MakeStaticLibTask( const std::string& output_pathname,
	                                  Iter begin,
	                                  Iter end,
	                                  const std::string& diagnostics_dir,
	                                  bool gnu,
	                                  bool debug )
	{
		const bool use_ar = gnu;
		
		Command link_command;
		
		link_command.push_back( use_ar ? "ar"  : "ld"      );
		link_command.push_back( use_ar ? "rcs" : "-static" );
		
		if ( !use_ar )
		{
			if ( !debug )
			{
				link_command.push_back( "-s" );
			}
			
			link_command.push_back( "-o" );
		}
		
		return TaskPtr( new LinkingTask( link_command, output_pathname, begin, end, diagnostics_dir ) );
	}
	
	void LinkProduct( const Project&     project,
	                  const TargetInfo&  targetInfo,
	                  const TaskPtr&     source_dependency )
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
		
		std::string outputDir = hasStaticLib ? libsDir : ProjectOutputDirPath( project.Name() );
		
		std::string exeDir = outputDir;
		
		if ( bundle )
		{
			std::string bundleName = linkName + ".app";
			
			CreateAppBundle( outputDir, bundleName );
			
			std::string contents( outputDir / bundleName / "Contents" );
			
			exeDir = contents / "MacOS";
			
			WritePkgInfo( contents / "PkgInfo", "APPL" + project.CreatorCode() );
		}
		
		std::string outFile = exeDir / linkName;
		
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
		
		std::vector< std::string > link_input_arguments( objectFiles.begin() + n_tools,
		                                                 objectFiles.end() );
		
		TaskPtr base_task;
		
		if ( hasStaticLib )
		{
			base_task = MakeStaticLibTask( outFile,
			                               objectFiles.begin() + n_tools,
			                               objectFiles.end(),
			                               diagnosticsDir,
			                               gnu,
			                               targetInfo.build == buildDebug );
		}
		else
		{
			base_task.reset( new NullTask() );
		}
		
		source_dependency->AddDependent( base_task );
		
		if ( !hasExecutable )
		{
			return;
		}
		
		if ( toolkit )
		{
			link_input_arguments.clear();  // don't link against common .o files
			
			link_input_arguments.push_back( "" );  // the tool .o file, later
			
			link_input_arguments.push_back( outFile );  // the static library
		}
		
		TaskPtr link_dependency_task( new NullTask() );
		
		base_task->AddDependent( link_dependency_task );
		
		// A copy so we can munge it
		std::vector< ProjName > usedProjects = project.AllUsedProjects();
		
		usedProjects.pop_back();  // we're last; drop us
		
		RemoveNonLibs( usedProjects );
		
		std::string libsDirOption;
		
		if ( !usedProjects.empty() )
		{
			libsDirOption = "-L" + libsDir;
			
			link_input_arguments.push_back( libsDirOption );
			
			link_dependency_task->UpdateInputStamp( LatestLibraryModificationDate( usedProjects ) );
			
			AddLibraryLinkArgs( usedProjects, link_input_arguments );
		}
		
		std::vector< std::string > allImports;
		
		// FIXME:  This is a hack
		if ( !gnu )
		{
			allImports = GetAllImports( project );
			
			AddImports( allImports, link_input_arguments );
		}
		
		if ( machO )
		{
			bool has_frameworks = !project.Frameworks().empty();
			
			AddFrameworks( has_frameworks ? project.Frameworks()
			                              : std::vector< std::string >( 1, "Carbon" ),
			               link_input_arguments );
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
			typedef std::vector< std::string >::const_iterator Iter;
			
			const Iter end = objectFiles.begin() + n_tools;
			
			for ( Iter it = objectFiles.begin();  it != end;  ++it )
			{
				const std::string& objectFile = *it;
				
				std::string linkOutput = outputDir / io::get_filename( objectFile );
				
				linkOutput.resize( linkOutput.size() - 2 );  // truncate ".o"
				
				link_input_arguments.front() = objectFile;
				
				TaskPtr link_tool_task( new LinkingTask( command, linkOutput, link_input_arguments, diagnosticsDir ) );
				
				link_tool_task->UpdateInputStamp( EffectiveModifiedDate( objectFile ) );
				
				link_dependency_task->AddDependent( link_tool_task );
			}
		}
		else
		{
			std::vector< FileName > rsrc_filenames = project.UsedRsrcFiles();
			
			std::vector< std::string > rsrc_pathnames( rsrc_filenames.size() );
			
			std::transform( rsrc_filenames.begin(), 
			                rsrc_filenames.end(),
			                rsrc_pathnames.begin(),
			                std::ptr_fun( RezLocation ) );
			
			bool usingOSXRez = gnu;
			
			TaskPtr rez_task;
			
			if ( !project.UsedRezFiles().empty() )
			{
				std::string rez_output_pathname = RezzedDirPath() / project.Name() + ".rsrc";
				
				rsrc_pathnames.push_back( rez_output_pathname );
				
				rez_task = MakeRezTask( project, rez_output_pathname, needCarbResource, usingOSXRez );
			}
			else
			{
				rez_task.reset( new NullTask() );
			}
			
			AddReadyTask( rez_task );
			
			TaskPtr link_task( new LinkingTask( command, outFile, link_input_arguments, diagnosticsDir ) );
			
			link_dependency_task->AddDependent( link_task );
			
			if ( !rsrc_pathnames.empty() )
			{
				std::string rsrcFile = gnu ? outputDir / BundleResourceFileRelativePath( linkName )
				                           : outFile;
				
				TaskPtr copy_rsrcs( new ResourceCopyingTask( rsrc_pathnames, rsrcFile, usingOSXRez ) );
				
				if ( bundle )
				{
					rez_task->AddDependent( copy_rsrcs );
				}
				else
				{
					rez_task->AddDependent( link_task );
					
					link_task->AddDependent( copy_rsrcs );
				}
			}
		}
	}
	
}

