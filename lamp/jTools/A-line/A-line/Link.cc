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

// plus
#include "plus/functional_extensions.hh"
#include "plus/pointer_to_function.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/pread.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Commands.hh"
#include "A-line/derived_filename.hh"
#include "A-line/Locations.hh"
#include "A-line/Project.hh"
#include "A-line/ProjectCommon.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	class RemoveDirTask : public Task
	{
		private:
			plus::string its_pathname;
		
		public:
			RemoveDirTask( const plus::string& pathname ) : its_pathname( pathname )
			{
			}
			
			void Start();
	};
	
	void RemoveDirTask::Start()
	{
		(void) rmdir( its_pathname.c_str() );
	}
	
	
	static inline bool TargetingLamp( bool targetingLamp )
	{
		return !ALINE_UNIX_DEVELOPMENT  ||  ALINE_LAMP_DEVELOPMENT && targetingLamp;
	}
	
	
	static void AddProjectImports( const Project& project, Platform platform, std::vector< plus::string >& link_input_arguments )
	{
		const std::vector< plus::string >& used_project_names = project.AllUsedProjects();
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter the_name = used_project_names.begin();  the_name != used_project_names.end();  ++the_name )
		{
			const Project& used_project = GetProject( *the_name, platform );
			
			const std::vector< plus::string >& library_imports = used_project.LibImports();
			
			for ( Iter the_import = library_imports.begin();  the_import != library_imports.end();  ++the_import )
			{
				const plus::string& import = *the_import;
				
				if ( import[ 0 ] == '-' )
				{
					// Weak-linked, so it's a stub library.
					// Currently, all of these are system-provided, not project-local.
					
					link_input_arguments.push_back( "-wi" );
					link_input_arguments.push_back( import.c_str() + 1 );
				}
				else
				{
					plus::string library_pathname = used_project.ProjectFolder() / import;
					
					link_input_arguments.push_back( io::file_exists( library_pathname ) ? library_pathname : import );
				}
			}
		}
	}
	
	
	static void AddFrameworks( const std::vector< plus::string >& frameworkNames, std::vector< plus::string >& v )
	{
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = frameworkNames.begin();  it != frameworkNames.end();  ++it )
		{
			v.push_back( "-framework" );
			v.push_back( *it );
		}
	}
	
	static bool ProjectBuildsLib( const Project& project )
	{
		const ProductType product = project.Product();
		
		return product == productStaticLib  ||  product == productDropIn;
	}
	
	static void RemoveNonLibs( std::vector< plus::string >& usedProjects, Platform platform )
	{
		usedProjects.resize( std::remove_if( usedProjects.begin(),
		                                     usedProjects.end(),
		                                     plus::compose1( std::not1( plus::ptr_fun( ProjectBuildsLib ) ),
		                                                     std::bind2nd( plus::ptr_fun( GetProject ),
		                                                                   platform ) ) ) - usedProjects.begin() );
	}
	
	static plus::string DirCreate_Idempotent( const plus::string& dir )
	{
		if ( !io::item_exists( dir ) )
		{
			int made = mkdir( dir.c_str(), 0700 );
		}
		
		return dir;
	}
	
	static void CreateAppBundle( const plus::string& location, const plus::string& name )
	{
		plus::string package   = DirCreate_Idempotent( location / name );
		plus::string contents  = DirCreate_Idempotent(   package  / "Contents" );
		plus::string macOS     = DirCreate_Idempotent(     contents / "MacOS" );
		plus::string resources = DirCreate_Idempotent(     contents / "Resources" );
	}
	
	static bool check_file_contents( p7::fd_t fd, const plus::string& desired )
	{
		const size_t size = desired.size();
		
		const bool sizes_match = p7::fstat( fd ).st_size == size;
		
		if ( sizes_match )
		{
			plus::var_string actual;
			
			char* p = actual.reset( size );
			
			if ( p7::pread( fd, p, size, 0 ) == size )
			{
				return actual == desired;
			}
		}
		
		return false;
	}
	
	static void WritePkgInfo( const char* pathname, const plus::string& contents )
	{
		n::owned< p7::fd_t > pkgInfo = p7::open( pathname, p7::o_rdwr | p7::o_creat );
		
		const bool match = check_file_contents( pkgInfo, contents );
		
		if ( !match )
		{
			p7::write( pkgInfo, contents );
		}
	}
	
	
	static void AddLibraryLinkArgs( const std::vector< plus::string >& usedLibs, std::vector< plus::string >& v )
	{
		// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
		// Somehow, this is actually required to actually link anything on Unix.
		
		typedef std::vector< plus::string >::const_reverse_iterator Iter;
		
		for ( Iter it = usedLibs.rbegin();  it != usedLibs.rend();  ++it )
		{
			const plus::string& name = *it;
			
			v.push_back( "-l" + name );
		}
	}
	
	static void UpdateInputStamp( const TaskPtr& task, const plus::string& input_pathname )
	{
		task->UpdateInputStamp( p7::stat( input_pathname ).st_mtime );
	}
	
	static plus::string diagnostics_file_path( const plus::string&  dir_path,
	                                           const plus::string&  target_path )
	{
		return dir_path / io::get_filename( target_path ) + ".txt";
	}
	
	class LinkingTask : public CommandTask
	{
		private:
			const char* its_caption;
		
		public:
			template < class Iter >
			LinkingTask( const Command&       command,
			             const plus::string&  output,
			             Iter                 input_begin,
			             Iter                 input_end,
			             const plus::string&  diagnostics,
			             const char*          caption )
			: CommandTask( command,
			               output,
			               diagnostics_file_path( diagnostics, output ),
			               &*input_begin,
			               &*input_end ),
				its_caption( caption )
			{
			}
			
			void Make();
	};
	
	void LinkingTask::Make()
	{
		plus::string output_filename = io::get_filename( OutputPath() );
		
		ExecuteCommand( shared_from_this(), its_caption + output_filename, get_command(), get_diagnostics_file_path().c_str() );
	}
	
	static plus::string BundleResourceFileRelativePath( const plus::string& linkName )
	{
		plus::string  bundleName   = linkName + ".app";
		plus::string  rsrcFileName = linkName + ".rsrc";
		
		return bundleName / "Contents" / "Resources" / rsrcFileName;
	}
	
	class RezzingTask : public FileTask
	{
		private:
			std::vector< plus::string >  itsInputPathnames;
			plus::string                 itsIncludeDirPathname;
			bool                         itIsTargetingLamp;
		
		public:
			RezzingTask( const std::vector< plus::string >&  input,
			             const plus::string&                 output,
			             const plus::string&                 includeDir,
			             bool                                lamp )
			:
				FileTask             ( output     ),
				itsInputPathnames    ( input      ),
				itsIncludeDirPathname( includeDir ),
				itIsTargetingLamp    ( lamp       )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void RezzingTask::Make()
	{
		Command rezCommand;
		
		// Use OS X's Rez on OS X, even if we're targeting Lamp
		const bool use_OSX_Rez = TARGET_RT_MAC_MACHO;
		
		if ( use_OSX_Rez )
		{
			rezCommand.push_back( "/Developer/Tools/Rez" );
			rezCommand.push_back( "-i" );
			rezCommand.push_back( "/Developer/Headers/FlatCarbon" );
			
			if ( !TargetingLamp( itIsTargetingLamp ) )
			{
				// ... but only use the data fork for OS X apps
				rezCommand.push_back( "-useDF" );
			}
			
			rezCommand.push_back( "-d" );
			
			rezCommand.push_back( "aeut_RezTemplateVersion=1" );
		}
		else
		{
			rezCommand.push_back( "mpwrez" );
		}
		
		rezCommand.push_back( "Types.r" );
		
		rezCommand.push_back( "-i" );
		
		rezCommand.push_back( itsIncludeDirPathname.c_str() );
		
		AugmentCommand( rezCommand, OutputOption( OutputPath().c_str() ) );
		
		AugmentCommand( rezCommand, itsInputPathnames );
		
		rezCommand.push_back( NULL );
		
		ExecuteCommand( shared_from_this(), "REZ   " + io::get_filename( OutputPath() ), rezCommand );
	}
	
	void RezzingTask::Return( bool succeeded )
	{
		// check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	static plus::string Project_FindResourceFile( const Project& project, const plus::string& filespec )
	{
		return project.FindResourceFile( filespec );
	}
	
	static TaskPtr MakeRezTask( const Project&       project,
	                            const plus::string&  output_pathname,
	                            bool                 needsCarbResource,
	                            bool                 lamp )
	{
		const std::vector< plus::string >& input_filenames = project.UsedRezFiles();
		
		std::vector< plus::string > input_pathnames( input_filenames.size() );
		
		std::transform( input_filenames.begin(),
		                input_filenames.end(),
		                input_pathnames.begin(),
		                std::bind1st( plus::ptr_fun( &Project_FindResourceFile ), project ) );
		
		if ( needsCarbResource )
		{
			input_pathnames.push_back( project.FindResourceFile( "Pedestal:CarbonApp.r" ) );
		}
		
		const plus::string& includeDir = project.ProjectFolder();
		
		TaskPtr rez_task = seize_ptr( new RezzingTask( input_pathnames,
		                                               output_pathname,
		                                               includeDir,
		                                               lamp ) );
		
		std::for_each( input_pathnames.begin(),
		               input_pathnames.end(),
		               std::bind1st( plus::ptr_fun( UpdateInputStamp ), rez_task ) );
		
		return rez_task;
	}
	
	class ResourceCopyingTask : public FileTask
	{
		private:
			std::vector< plus::string >  itsInputPathnames;
			bool                         itIsTargetingLamp;
		
		public:
			ResourceCopyingTask( const std::vector< plus::string >&  input,
			                     const plus::string&                 output,
			                     bool                                lamp )
			:
				FileTask         ( output   ),
				itsInputPathnames( input    ),
				itIsTargetingLamp( lamp     )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void ResourceCopyingTask::Make()
	{
		Command command;
		
		command.push_back( "cpres" );
		
		if ( !TargetingLamp( itIsTargetingLamp ) )
		{
			command.push_back( "--data" );
		}
		
		AugmentCommand( command, itsInputPathnames );
		
		command.push_back( OutputPath().c_str() );
		
		command.push_back( NULL );
		
		ExecuteCommand( shared_from_this(), "RSRC  " + io::get_filename( OutputPath() ), command );
	}
	
	void ResourceCopyingTask::Return( bool succeeded )
	{
		// check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	static void make_task_depend_on_libs( const TaskPtr&                      task,
	                                      const std::vector< plus::string >&  used_project_names,
	                                      Platform                            platform )
	{
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = used_project_names.begin();  it != used_project_names.end();  ++it )
		{
			const plus::string& name = *it;
			
			const Project& project  = GetProject( name, platform );
			
			ASSERT( !project.get_static_lib_task().expired() );
			
			project.get_static_lib_task().lock()->AddDependent( task );
		}
	}
	
	template < class Iter >
	static TaskPtr make_link_task( const Command&       link_command,
	                               const plus::string&  output_path,
	                               Iter                 begin,
	                               Iter                 end,
	                               const plus::string&  diagnostics_dir,
	                               const char*          caption = "LINK  " )
	{
		return seize_ptr( new LinkingTask( link_command,
		                                   output_path,
		                                   begin,
		                                   end,
		                                   diagnostics_dir,
		                                   caption ) );
	}
	
	template < class Container >
	static inline TaskPtr make_link_task( const Command&       link_command,
	                                      const plus::string&  output_path,
	                                      const Container&     input,
	                                      const plus::string&  diagnostics_dir,
	                                      const char*          caption = "LINK  " )
	{
		return seize_ptr( new LinkingTask( link_command,
		                                   output_path,
		                                   input.begin(),
		                                   input.end(),
		                                   diagnostics_dir,
		                                   caption ) );
	}
	
	template < class Iter >
	static TaskPtr MakeStaticLibTask( const plus::string&  output_pathname,
	                                  Iter                 begin,
	                                  Iter                 end,
	                                  const plus::string&  diagnostics_dir )
	{
		Command link_command;
		
		link_command.push_back( "ar"  );
		link_command.push_back( "rcs" );
		
		return make_link_task( link_command,
		                       output_pathname,
		                       begin,
		                       end,
		                       diagnostics_dir,
		                       "AR    " );
	}
	
	
	class object_filename_filler
	{
		private:
			const plus::string& its_objects_dir;
		
		public:
			object_filename_filler( const plus::string& objects )
			:
				its_objects_dir( objects )
			{
				ASSERT( !its_objects_dir.empty() );
			}
			
			plus::string operator()( const plus::string& source_path ) const
			{
				const char* extension = ".o";
				
				return derived_pathname( its_objects_dir, source_path, extension );
			}
	};
	
	static void FillObjectFiles( const plus::string&                 objects_dir,
	                             const std::vector< plus::string >&  source_paths,
	                             std::vector< plus::string >&        object_pathnames )
	{
		std::transform( source_paths.begin(),
		                source_paths.end(),
		                object_pathnames.begin(),
		                object_filename_filler( objects_dir ) );
	}
	
	void NameObjectFiles( const Project&                project,
	                      std::vector< plus::string >&  object_pathnames )
	{
		plus::string objects_dir = ProjectObjectsDirPath( project.Name() );
		
		const std::vector< plus::string >& sources = project.Sources();
		
		object_pathnames.resize( sources.size() );
		
		FillObjectFiles( objects_dir, sources, object_pathnames );
	}
	
	
	void LinkProduct( Project&                       project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 project_base_task,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies )
	{
		const bool lamp = TargetingLamp( targetInfo.envType == envLamp );
		
		const bool real_unix = !lamp;  // unix is an evil macro on Linux
		
		plus::string diagnosticsDir = ProjectDiagnosticsDirPath( project.Name() );
		
		TaskPtr rmdir_diagnostics_task = seize_ptr( new RemoveDirTask( diagnosticsDir ) );
		
		std::vector< plus::string > objectFiles;
		
		NameObjectFiles( project, objectFiles );
		
		const std::size_t n_tools = project.ToolCount();
		
		const ProductType product = project.Product();
		
		const bool exports_static_lib = product == productStaticLib  ||  product == productDropIn;
		
		const bool hasStaticLib = exports_static_lib  ||  product == productToolkit;
		
		plus::string libsDir = LibrariesDirPath();
		
		plus::string library_pathname;
		
		TaskPtr lib_task;
		
		const bool non_empty_lib = hasStaticLib  &&  objectFiles.size() > n_tools;
		
		if ( non_empty_lib )
		{
			plus::string library_prefix    = real_unix ? "lib" : "";
			plus::string library_extension = real_unix ? ".a" : ".lib";
			
			plus::string library_filename = library_prefix + project.Name() + library_extension;
			
			library_pathname = libsDir / library_filename;
			
			lib_task = MakeStaticLibTask( library_pathname,
			                              objectFiles.begin() + n_tools,
			                              objectFiles.end(),
			                              diagnosticsDir );
			
			source_dependency->AddDependent( lib_task );
		}
		
		if ( exports_static_lib )
		{
			if ( non_empty_lib )
			{
				project.set_static_lib_task( lib_task );
				
				lib_task->AddDependent( rmdir_diagnostics_task );
			}
			else
			{
				plus::var_string message = "\n" "# error: library project '";
				
				message += project.Name();
				
				message += "' has no source files\n";
				
				p7::write( p7::stderr_fileno, message );
				
				throw p7::exit_failure;
			}
			
			return;
		}
		
		
		CommandGenerator cmdgen( targetInfo );
		
		Command command;
		
		command.push_back( cmdgen.LinkerName() );
		
		AugmentCommand( command, cmdgen.TargetArchitecture() );
		
		plus::string driverName;
		
		const char* fileType = NULL;
		
		plus::string creator = project.CreatorCode();
		
	#if ALINE_MAC_DEVELOPMENT
		
		if ( creator.length() != sizeof 'Ctor' )
		{
			creator = "\?\?\?\?";
		}
		
	#endif
		
		switch ( product )
		{
			case productToolkit:
			case productTool:
				break;
			
			case productApplication:
				fileType = "APPL";
				break;
			
		#if ALINE_LAMP_DEVELOPMENT
			
			case productSharedLib:
				command.push_back( cmdgen.MWTargetSharedLibrary() );
				break;
			
			case productINIT:
				fileType = "INIT";
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "INIT=0" ) );
				break;
			
			case productDriver:
				driverName = "." + project.Name();
				
				fileType = "DRVR";
				creator  = "RSED";
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "DRVR=0" ) );
				AugmentCommand( command, cmdgen.ResourceName     ( driverName.c_str() ) );
				
				command.push_back( cmdgen.CustomDriverHeader() );
				break;
			
		#endif
			
			default:
				p7::write( p7::stderr_fileno, STR_LEN( "Sorry, this product can't be built on this platform.\n" ) );
				
				p7::throw_errno( EINVAL );
		}
		
		
		const bool toolkit = n_tools > 0;
		
		TaskPtr link_dependency_task = seize_ptr( new NullTask() );
		
		std::vector< plus::string > link_input_arguments;
		
		if ( toolkit )
		{
			link_input_arguments.push_back( "" );  // the tool .o file, later
			
			if ( non_empty_lib )
			{
				link_input_arguments.push_back( library_pathname );  // the static library
			}
		}
		else
		{
			link_input_arguments.insert( link_input_arguments.begin(),
			                             objectFiles.begin(),
			                             objectFiles.end() );
		}
		
		const TaskPtr& link_prerequisite = non_empty_lib ? lib_task
		                                                 : source_dependency;
		
		link_prerequisite->AddDependent( link_dependency_task );
		
		// A copy so we can munge it
		std::vector< plus::string > usedProjects = project.AllUsedProjects();
		
		usedProjects.pop_back();  // we're last; drop us
		
		RemoveNonLibs( usedProjects, targetInfo.platform );
		
		plus::string libsDirOption;
		
		if ( !usedProjects.empty() )
		{
			libsDirOption = "-L" + libsDir;
			
			link_input_arguments.push_back( libsDirOption );
			
			make_task_depend_on_libs( link_dependency_task, usedProjects, project.get_platform() );
			
			AddLibraryLinkArgs( usedProjects, link_input_arguments );
		}
		
		// FIXME:  This is a hack
		if ( lamp )
		{
			AddProjectImports( project, targetInfo.platform, link_input_arguments );
		}
		
		const bool machO = real_unix && targetInfo.platform & runtimeMachO;
		
		if ( machO )
		{
			bool has_frameworks = !project.Frameworks().empty();
			
			AddFrameworks( has_frameworks ? project.Frameworks()
			                              : std::vector< plus::string >( 1, "Carbon" ),
			               link_input_arguments );
		}
		
		
		if ( cmdgen.LinkerOptions()[0] )
		{
			command.push_back( cmdgen.LinkerOptions() );
		}
		
		plus::string outputDir = ProjectOutputDirPath( project.Name() );
		
		command.push_back( "-o" );
		
		if ( toolkit )
		{
			typedef std::vector< plus::string >::const_iterator Iter;
			
			const Iter end = objectFiles.begin() + n_tools;
			
			std::vector< TaskPtr >::const_iterator the_task = tool_dependencies.begin();
			
			for ( Iter the_object = objectFiles.begin();  the_object != end;  ++the_object, ++the_task )
			{
				const plus::string& objectFile = *the_object;
				
				plus::var_string linkOutput = outputDir / io::get_filename( objectFile );
				
				linkOutput.resize( linkOutput.size() - 2 );  // truncate ".o"
				
				// truncate ".c" or ".cc"
				linkOutput.resize( linkOutput.find_last_of( "." ) );
				
				link_input_arguments.front() = objectFile;
				
				TaskPtr link_tool_task = make_link_task( command,
				                                         linkOutput,
				                                         link_input_arguments,
				                                         diagnosticsDir );
				
				(*the_task)->AddDependent( link_tool_task );
				
				link_dependency_task->AddDependent( link_tool_task );
				
				link_tool_task->AddDependent( rmdir_diagnostics_task );
			}
		}
		else
		{
			plus::string exeDir = outputDir;
			
			plus::string linkName = project.ProgramName();
			
			if ( linkName == "" )
			{
				linkName = project.Name();
			}
			
			const bool app = ALINE_MAC_DEVELOPMENT  &&  product == productApplication;
			
			const bool bundle = real_unix && app;
			
			if ( ALINE_MAC_DEVELOPMENT )
			{
				plus::string pkginfo_dir;
				
				if ( bundle )
				{
					plus::string bundleName = linkName + ".app";
					
					CreateAppBundle( outputDir, bundleName );
					
					plus::string contents( outputDir / bundleName / "Contents" );
					
					exeDir = contents / "MacOS";
					
					pkginfo_dir = contents;
				}
				else if ( fileType != NULL )
				{
					pkginfo_dir = ProjectMetadataDirPath( project.Name() );
				}
				
				if ( fileType != NULL )
				{
					plus::string pkginfo = pkginfo_dir / "PkgInfo";
					
					WritePkgInfo( pkginfo.c_str(), fileType + creator );
					
					if ( lamp )
					{
						link_input_arguments.push_back( pkginfo );
					}
				}
			}
			
			plus::string outFile = exeDir / linkName;
			
			TaskPtr link_task = make_link_task( command,
			                                    outFile,
			                                    link_input_arguments,
			                                    diagnosticsDir );
			
			link_dependency_task->AddDependent( link_task );
			
			link_task->AddDependent( rmdir_diagnostics_task );
			
			if ( ALINE_MAC_DEVELOPMENT )
			{
				const std::vector< plus::string >& rsrc_filenames = project.UsedRsrcFiles();
				
				std::vector< plus::string > rsrc_pathnames( rsrc_filenames.size() );
				
				std::transform( rsrc_filenames.begin(), 
				                rsrc_filenames.end(),
				                rsrc_pathnames.begin(),
		                        std::bind1st( plus::ptr_fun( &Project_FindResourceFile ), project ) );
				
				TaskPtr rez_task;
				
				const Platform carbonCFM = apiMacCarbon | runtimeCodeFragments;
				
				const bool needsCarbResource = product == productApplication  &&  (targetInfo.platform & carbonCFM) == carbonCFM;
				
				if ( needsCarbResource || !project.UsedRezFiles().empty() )
				{
					plus::string rez_output_pathname = RezzedDirPath() / project.Name() + ".rsrc";
					
					rsrc_pathnames.push_back( rez_output_pathname );
					
					rez_task = MakeRezTask( project, rez_output_pathname, needsCarbResource, lamp );
				}
				else
				{
					rez_task = seize_ptr( new NullTask() );
				}
				
				project_base_task->AddDependent( rez_task );
				
				if ( !rsrc_pathnames.empty() )
				{
					plus::string rsrcFile = bundle ? outputDir / BundleResourceFileRelativePath( linkName )
					                               : outFile;
					
					TaskPtr copy_rsrcs = seize_ptr( new ResourceCopyingTask( rsrc_pathnames,
					                                                         rsrcFile,
					                                                         lamp ) );
					
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
	
}

