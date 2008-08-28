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
#include "POSeven/Stat.hh"

// MoreFunctional
#include "FunctionalExtensions.hh"
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
	
	
	inline bool TargetingLamp( bool targetingLamp )
	{
		return !ALINE_UNIX_DEVELOPMENT  ||  ALINE_LAMP_DEVELOPMENT && targetingLamp;
	}
	
	
	static void AddProjectImports( const Project& project, std::vector< std::string >& link_input_arguments )
	{
		const std::vector< std::string >& used_project_names = project.AllUsedProjects();
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter the_name = used_project_names.begin();  the_name != used_project_names.end();  ++the_name )
		{
			const Project& used_project = GetProject( *the_name );
			
			const std::vector< std::string >& library_imports = used_project.LibImports();
			
			for ( Iter the_import = library_imports.begin();  the_import != library_imports.end();  ++the_import )
			{
				const std::string& import = *the_import;
				
				if ( import[ 0 ] == '-' )
				{
					// Weak-linked, so it's a stub library.
					// Currently, all of these are system-provided, not project-local.
					
					link_input_arguments.push_back( "-wi" );
					link_input_arguments.push_back( import.c_str() + 1 );
				}
				else
				{
					std::string library_pathname = used_project.ProjectFolder() / import;
					
					link_input_arguments.push_back( io::file_exists( library_pathname ) ? library_pathname : import );
				}
			}
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
	
	class LinkingTask : public FileTask
	{
		private:
			Command                     itsCommand;
			std::vector< std::string >  itsInputArguments;
			std::string                 itsDiagnosticsDir;
		
		public:
			LinkingTask( const Command&                     command,
			             const std::string&                 output,
			             const std::vector< std::string >&  input,
			             const std::string&                 diagnostics ) : FileTask         ( output      ),
			                                                                itsCommand       ( command     ),
			                                                                itsInputArguments( input       ),
			                                                                itsDiagnosticsDir( diagnostics )
			{
			}
			
			template < class Iter >
			LinkingTask( const Command&      command,
			             const std::string&  output,
			             Iter                input_begin,
			             Iter                input_end,
			             const std::string&  diagnostics ) : FileTask         ( output      ),
			                                                 itsCommand       ( command     ),
			                                                 itsInputArguments( input_begin, input_end ),
			                                                 itsDiagnosticsDir( diagnostics )
			{
			}
			
			void Make();
	};
	
	void LinkingTask::Make()
	{
		itsCommand.push_back( OutputPathname().c_str() );
		
		AugmentCommand( itsCommand, itsInputArguments );
		
		std::string output_filename = io::get_filename_string( OutputPathname() );
		
		itsCommand.push_back( NULL );
		
		std::string diagnostics_pathname = itsDiagnosticsDir / DiagnosticsFilenameFromSourceFilename( output_filename );
		
		RunCommand( itsCommand, diagnostics_pathname.c_str(), "Linking: " + output_filename );
	}
	
	static std::string BundleResourceFileRelativePath( const std::string& linkName )
	{
		std::string bundleName   = linkName + ".app";
		std::string rsrcFileName = linkName + ".rsrc";
		
		return bundleName / "Contents" / "Resources" / rsrcFileName;
	}
	
	class RezzingTask : public FileTask
	{
		private:
			std::vector< std::string >  itsInputPathnames;
			std::string                 itsIncludeDirPathname;
			bool                        itIsTargetingLamp;
		
		public:
			RezzingTask( const std::vector< std::string >&  input,
			             const std::string&                 output,
			             const std::string&                 includeDir,
			             bool                               lamp ) : FileTask             ( output     ),
			                                                         itsInputPathnames    ( input      ),
			                                                         itsIncludeDirPathname( includeDir ),
			                                                         itIsTargetingLamp    ( lamp       )
			{
			}
			
			void Make();
	};
	
	void RezzingTask::Make()
	{
		Command rezCommand;
		
		if ( TargetingLamp( itIsTargetingLamp ) )
		{
			rezCommand.push_back( "mpwrez" );
			rezCommand.push_back( "-c" );
			rezCommand.push_back( "RSED" );
			rezCommand.push_back( "-t" );
			rezCommand.push_back( "rsrc" );
		}
		else
		{
			rezCommand.push_back( "/Developer/Tools/Rez" );
			rezCommand.push_back( "-i" );
			rezCommand.push_back( "/Developer/Headers/FlatCarbon" );
			rezCommand.push_back( "-useDF" );
		}
		
		rezCommand.push_back( "-i" );
		
		rezCommand.push_back( itsIncludeDirPathname.c_str() );
		
		AugmentCommand( rezCommand, OutputOption( OutputPathname().c_str() ) );
		
		AugmentCommand( rezCommand, itsInputPathnames );
		
		rezCommand.push_back( NULL );
		
		RunCommand( rezCommand, NULL, "Rezzing: " + io::get_filename_string( OutputPathname() ) );
	}
	
	static TaskPtr MakeRezTask( const Project&      project,
	                            const std::string&  output_pathname,
	                            bool                needsCarbResource,
	                            bool                lamp )
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
		
		TaskPtr rez_task( new RezzingTask( input_pathnames, output_pathname, includeDir, lamp ) );
		
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
	
	template < class F, class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "", F f = F() )
	{
		if ( begin == end )
		{
			return "";
		}
		
		std::string result = f( *begin++ );
		
		while ( begin != end )
		{
			result += glue;
			result += f( *begin++ );
		}
		
		return result;
	}
	
	template < class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "" )
	{
		return join( begin, end, glue, more::identity< std::string >() );
	}
	
	class ResourceCopyingTask : public FileTask
	{
		private:
			std::vector< std::string >  itsInputPathnames;
			bool                        itIsTargetingLamp;
		
		public:
			ResourceCopyingTask( const std::vector< std::string >&  input,
			                     const std::string&                 output,
			                     bool                               lamp ) : FileTask         ( output   ),
			                                                                 itsInputPathnames( input    ),
			                                                                 itIsTargetingLamp( lamp     )
			{
			}
			
			void Make();
	};
	
	void ResourceCopyingTask::Make()
	{
		Command command;
		
		std::string command_line;
		
		if ( TargetingLamp( itIsTargetingLamp ) )
		{
			command.push_back( "cpres" );
			
			AugmentCommand( command, itsInputPathnames );
			
			command.push_back( OutputPathname().c_str() );
		}
		else
		{
			command_line = join( itsInputPathnames.begin(),
			                     itsInputPathnames.end(),
			                     "; ",
			                     std::ptr_fun( MakeEchoedRezInclude ) );
			
			command_line = paren( command_line );
			
			command_line += " | /Developer/Tools/Rez -append -useDF -o ";
			command_line += q( OutputPathname() );
			
			command.push_back( "/bin/sh" );
			command.push_back( "-c" );
			command.push_back( command_line.c_str() );
		}
		
		command.push_back( NULL );
		
		RunCommand( command, "", "Copying resources: " + io::get_filename_string( OutputPathname() ) );
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
	static TaskPtr MakeStaticLibTask( const std::string&  output_pathname,
	                                  Iter                begin,
	                                  Iter                end,
	                                  const std::string&  diagnostics_dir )
	{
		Command link_command;
		
		link_command.push_back( "ar"  );
		link_command.push_back( "rcs" );
		
		return TaskPtr( new LinkingTask( link_command, output_pathname, begin, end, diagnostics_dir ) );
	}
	
	
	static std::size_t PartitionSourceFiles( const Project& project, std::vector< std::string >& sourceFiles )
	{
		if ( project.Product() != productToolkit )
		{
			return 0;
		}
		
		std::vector< std::string > toolSourceFiles = project.ToolSourceFiles();
		
		std::sort( toolSourceFiles.begin(), toolSourceFiles.end() );
		
		std::size_t n_tools = std::partition( sourceFiles.begin(),
		                                      sourceFiles.end(),
		                                      std::bind2nd( more::ptr_fun( &SourceFileIsTool ),
		                                                                    toolSourceFiles ) ) - sourceFiles.begin();
		
		return n_tools;
	}
	
	static void FillObjectFiles( const std::string&                 objects_dir,
	                             const std::vector< std::string >&  source_paths,
	                             std::vector< std::string >&        object_pathnames )
	{
		std::transform( source_paths.begin(),
		                source_paths.end(),
		                object_pathnames.begin(),
		                more::compose1( std::bind1st( more::ptr_fun( static_cast< std::string (*)( const std::string&, const std::string& ) >( operator/ ) ),
		                                              objects_dir ),
		                                more::compose1( more::ptr_fun( ObjectFileName ),
		                                                more::ptr_fun( static_cast< std::string (*)( const std::string& ) >( io::get_filename ) ) ) ) );
	}
	
	std::size_t NameObjectFiles( const Project&               project,
	                             std::vector< std::string >&  source_paths,
	                             std::vector< std::string >&  object_pathnames )
	{
		std::size_t n_tools = 0;
		
		bool toolkit = project.Product() == productToolkit;
		
		if ( toolkit )
		{
			source_paths = project.Sources();
			
			n_tools = PartitionSourceFiles( project, source_paths );
		}
		
		std::string objects_dir = ProjectObjectsDirPath( project.Name() );
		
		const std::vector< std::string >& sources = toolkit ? source_paths : project.Sources();
		
		object_pathnames.resize( sources.size() );
		
		FillObjectFiles( objects_dir, sources, object_pathnames );
		
		return n_tools;
	}
	
	static std::size_t NameObjectFiles( const Project&               project,
	                                    std::vector< std::string >&  object_pathnames )
	{
		std::vector< std::string > source_paths;
		
		return NameObjectFiles( project, source_paths, object_pathnames );
	}
	
	
	void LinkProduct( const Project&                 project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies )
	{
		const bool lamp = TargetingLamp( targetInfo.envType == envLamp );
		
		const bool unix = !lamp;
		
		gLibraryPrefix    = unix ? "lib" : "";
		gLibraryExtension = unix ? ".a" : ".lib";
		
		std::string diagnosticsDir = ProjectDiagnosticsDirPath( project.Name() );
		
		std::vector< std::string > objectFiles;
		
		const std::size_t n_tools = NameObjectFiles( project, objectFiles );
		
		const bool hasStaticLib = project.Product() == productStaticLib  ||  project.Product() == productToolkit;
		
		std::string libsDir = LibrariesDirPath();
		
		std::string library_pathname;
		
		TaskPtr base_task;
		
		if ( hasStaticLib )
		{
			std::string library_filename = gLibraryPrefix + project.Name() + gLibraryExtension;
			
			library_pathname = libsDir / library_filename;
			
			base_task = MakeStaticLibTask( library_pathname,
			                               objectFiles.begin() + n_tools,
			                               objectFiles.end(),
			                               diagnosticsDir );
		}
		else
		{
			base_task.reset( new NullTask() );
		}
		
		source_dependency->AddDependent( base_task );
		
		if ( project.Product() == productStaticLib )
		{
			return;
		}
		
		
		CommandGenerator cmdgen( targetInfo );
		
		Command command;
		
		command.push_back( cmdgen.LinkerName() );
		
		AugmentCommand( command, cmdgen.TargetArchitecture() );
		
		std::string driverName;
		
		switch ( project.Product() )
		{
			case productToolkit:
			case productApplication:
			case productTool:
				break;
			
		#if ALINE_LAMP_DEVELOPMENT
			
			case productSharedLib:
				command.push_back( cmdgen.MWTargetSharedLibrary() );
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
			
		#endif
			
			default:
				p7::write( p7::stderr_fileno, STR_LEN( "Sorry, this product can't be built on this platform.\n" ) );
				
				p7::throw_errno( EINVAL );
		}
		
		
		const bool toolkit = n_tools > 0;
		
		std::vector< std::string > link_input_arguments;
		
		if ( toolkit )
		{
			link_input_arguments.push_back( "" );  // the tool .o file, later
			
			link_input_arguments.push_back( library_pathname );  // the static library
		}
		else
		{
			link_input_arguments.insert( link_input_arguments.begin(),
			                             objectFiles.begin(),
			                             objectFiles.end() );
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
		
		// FIXME:  This is a hack
		if ( lamp )
		{
			AddProjectImports( project, link_input_arguments );
		}
		
		const bool machO = unix && targetInfo.platform & CD::runtimeMachO;
		
		if ( machO )
		{
			bool has_frameworks = !project.Frameworks().empty();
			
			AddFrameworks( has_frameworks ? project.Frameworks()
			                              : std::vector< std::string >( 1, "Carbon" ),
			               link_input_arguments );
		}
		
		
		if ( lamp  &&  project.CreatorCode().size() > 0 )
		{
			AugmentCommand( command, cmdgen.OutputCreator( project.CreatorCode().c_str() ) );
		}
		
		if ( cmdgen.LinkerOptions()[0] )
		{
			command.push_back( cmdgen.LinkerOptions() );
		}
		
		std::string outputDir = ProjectOutputDirPath( project.Name() );
		
		command.push_back( "-o" );
		
		if ( toolkit )
		{
			typedef std::vector< std::string >::const_iterator Iter;
			
			const Iter end = objectFiles.begin() + n_tools;
			
			std::vector< TaskPtr >::const_iterator the_task = tool_dependencies.begin();
			
			for ( Iter the_object = objectFiles.begin();  the_object != end;  ++the_object, ++the_task )
			{
				const std::string& objectFile = *the_object;
				
				std::string linkOutput = outputDir / io::get_filename( objectFile );
				
				linkOutput.resize( linkOutput.size() - 2 );  // truncate ".o"
				
				link_input_arguments.front() = objectFile;
				
				TaskPtr link_tool_task( new LinkingTask( command, linkOutput, link_input_arguments, diagnosticsDir ) );
				
				(*the_task)->AddDependent( link_tool_task );
				
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
			
			TaskPtr rez_task;
			
			const CD::Platform carbonCFM = CD::apiMacCarbon | CD::runtimeCodeFragments;
			
			const bool needsCarbResource = project.Product() == productApplication  &&  (targetInfo.platform & carbonCFM) == carbonCFM;
			
			if ( needsCarbResource || !project.UsedRezFiles().empty() )
			{
				std::string rez_output_pathname = RezzedDirPath() / project.Name() + ".rsrc";
				
				rsrc_pathnames.push_back( rez_output_pathname );
				
				rez_task = MakeRezTask( project, rez_output_pathname, needsCarbResource, lamp );
			}
			else
			{
				rez_task.reset( new NullTask() );
			}
			
			AddReadyTask( rez_task );
			
			std::string exeDir = outputDir;
			
			std::string linkName = project.ProgramName();
			
			if ( linkName == "" )
			{
				linkName = project.Name();
			}
			
			const bool app = project.Product() == productApplication;
			
			const bool bundle = unix && app;
			
			std::string pkginfo_dir;
			
			if ( bundle )
			{
				std::string bundleName = linkName + ".app";
				
				CreateAppBundle( outputDir, bundleName );
				
				std::string contents( outputDir / bundleName / "Contents" );
				
				exeDir = contents / "MacOS";
				
				pkginfo_dir = contents;
			}
			else if ( app )
			{
				pkginfo_dir = ProjectMetadataDirPath( project.Name() );
			}
			
			if ( app )
			{
				std::string pkginfo = pkginfo_dir / "PkgInfo";
				
				WritePkgInfo( pkginfo, "APPL" + project.CreatorCode() );
				
				if ( lamp )
				{
					link_input_arguments.push_back( pkginfo );
				}
			}
			
			std::string outFile = exeDir / linkName;
			
			TaskPtr link_task( new LinkingTask( command, outFile, link_input_arguments, diagnosticsDir ) );
			
			link_dependency_task->AddDependent( link_task );
			
			if ( !rsrc_pathnames.empty() )
			{
				std::string rsrcFile = bundle ? outputDir / BundleResourceFileRelativePath( linkName )
				                              : outFile;
				
				TaskPtr copy_rsrcs( new ResourceCopyingTask( rsrc_pathnames, rsrcFile, lamp ) );
				
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

