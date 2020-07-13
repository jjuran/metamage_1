/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <stdlib.h>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/string/concat.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/basename.hh"
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
#include "A-line/Info-plist.hh"
#include "A-line/Locations.hh"
#include "A-line/prefix.hh"
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
	
	
	static inline bool TargetingRelix( bool targetingRelix )
	{
		return !ALINE_UNIX_DEVELOPMENT  ||  ALINE_RELIX_DEVELOPMENT && targetingRelix;
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
				
				if ( import[ 0 ] == '@' )
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
	
	struct not_a_lib_on
	{
		Platform platform;
		
		not_a_lib_on( Platform platform ) : platform( platform )
		{
		}
		
		bool operator()( const plus::string& name )
		{
			return !ProjectBuildsLib( GetProject( name, platform ) );
		}
	};
	
	static void RemoveNonLibs( std::vector< plus::string >& usedProjects, Platform platform )
	{
		usedProjects.resize( std::remove_if( usedProjects.begin(),
		                                     usedProjects.end(),
		                                     not_a_lib_on( platform ) ) - usedProjects.begin() );
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
			plus::string actual;
			
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
		return dir_path / p7::basename( target_path ) + ".txt";
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
	
	static bool is_dot_a( const plus::string& s )
	{
		const size_t size = s.size();
		
		if ( size > 2 )
		{
			const char* data = s.data();
			
			return data[ size - 2 ] == '.'  &&  data[ size - 1 ] == 'a';
		}
		
		return false;
	}
	
	void LinkingTask::Make()
	{
		plus::string output_filename = p7::basename( OutputPath() );
		
	#ifndef __RELIX__
		
		/*
			Delete unix static library files before linking, since `ar rcs`
			always appends and never replaces, and there's no option for it to
			do that.
			
			Don't delete Metrowerks' files (ending in ".lib"), however, since
			it doesn't have this problem.
			
			In MacRelix-hosted builds, only Metrowerks is targeted, so don't
			even check.
		*/
		
		if ( is_dot_a( output_filename ) )
		{
			unlink( OutputPath().c_str() );
		}
		
	#endif
		
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
			bool                         itIsTargetingRelix;
		
		public:
			RezzingTask( const std::vector< plus::string >&  input,
			             const plus::string&                 output,
			             const plus::string&                 includeDir,
			             bool                                relix )
			:
				FileTask             ( output     ),
				itsInputPathnames    ( input      ),
				itsIncludeDirPathname( includeDir ),
				itIsTargetingRelix   ( relix      )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void RezzingTask::Make()
	{
		Command rezCommand;
		
	#ifdef __APPLE__
		
		// Use OS X's Rez on OS X, even if we're targeting MacRelix
		const bool use_OSX_Rez = true;
		
	#else
		
		const bool use_OSX_Rez = false;
		
	#endif
		
		if ( use_OSX_Rez )
		{
			const char* rez = "/Developer/Tools/Rez";
			
			struct stat sb;
			
			if ( stat( rez, &sb ) != 0 )
			{
				rez = "Rez";  // It's at /usr/bin/Rez on Lion.
			}
			
			rezCommand.push_back( rez );
			rezCommand.push_back( "-i" );
			rezCommand.push_back( "/Developer/Headers/FlatCarbon" );
			
			if ( !TargetingRelix( itIsTargetingRelix ) )
			{
				// ... but only use the data fork for OS X apps
				rezCommand.push_back( "-useDF" );
			}
		}
		else
		{
			rezCommand.push_back( "mpwrez" );
		}
		
		rezCommand.push_back( "-d" );
		
		rezCommand.push_back( "aeut_RezTemplateVersion=1" );
		
		rezCommand.push_back( "Carbon.r" );
		
		rezCommand.push_back( "-i" );
		
		rezCommand.push_back( itsIncludeDirPathname.c_str() );
		
		AugmentCommand( rezCommand, OutputOption( OutputPath().c_str() ) );
		
		AugmentCommand( rezCommand, itsInputPathnames );
		
		rezCommand.push_back( NULL );
		
		ExecuteCommand( shared_from_this(), "REZ   " + p7::basename( OutputPath() ), rezCommand );
	}
	
	void RezzingTask::Return( bool succeeded )
	{
		// check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	static TaskPtr MakeRezTask( const Project&       project,
	                            const plus::string&  output_pathname,
	                            bool                 needsCarbResource,
	                            bool                 relix )
	{
		const std::vector< plus::string >& input_filenames = project.UsedRezFiles();
		
		const size_t n = input_filenames.size();
		
		std::vector< plus::string > input_pathnames;
		
		input_pathnames.reserve( n );
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			const plus::string& filename = input_filenames[ i ];
			
			input_pathnames.push_back( project.FindResourceFile( filename ) );
		}
		
		if ( needsCarbResource )
		{
			input_pathnames.push_back( project.FindResourceFile( "Pedestal:CarbonApp.r" ) );
		}
		
		const plus::string& includeDir = project.ProjectFolder();
		
		TaskPtr rez_task = seize_ptr( new RezzingTask( input_pathnames,
		                                               output_pathname,
		                                               includeDir,
		                                               relix ) );
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			UpdateInputStamp( rez_task, input_pathnames[ i ] );
		}

		return rez_task;
	}
	
	class ResourceCopyingTask : public FileTask
	{
		private:
			std::vector< plus::string >  itsInputPathnames;
			bool                         itIsTargetingRelix;
		
		public:
			ResourceCopyingTask( const std::vector< plus::string >&  input,
			                     const plus::string&                 output,
			                     bool                                relix )
			:
				FileTask          ( output   ),
				itsInputPathnames ( input    ),
				itIsTargetingRelix( relix    )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void ResourceCopyingTask::Make()
	{
		Command command;
		
		command.push_back( "cpres" );
		
		if ( !TargetingRelix( itIsTargetingRelix ) )
		{
			command.push_back( "--data" );
		}
		
		AugmentCommand( command, itsInputPathnames );
		
		command.push_back( OutputPath().c_str() );
		
		command.push_back( NULL );
		
		ExecuteCommand( shared_from_this(), "RSRC  " + p7::basename( OutputPath() ), command );
	}
	
	void ResourceCopyingTask::Return( bool succeeded )
	{
		// check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	class ResourceFileCopyingTask : public FileTask
	{
		private:
			const plus::string its_input_pathname;
		
		public:
			ResourceFileCopyingTask( const plus::string&  input,
			                         const plus::string&  output )
			:
				FileTask          ( output ),
				its_input_pathname( input  )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void ResourceFileCopyingTask::Make()
	{
		Command command;
		
		command.push_back( "cp"                       );
		command.push_back( its_input_pathname.c_str() );
		command.push_back( OutputPath()      .c_str() );
		command.push_back( NULL                       );
		
		const plus::string message = "COPY  " + p7::basename( OutputPath() );
		
		ExecuteCommand( shared_from_this(), message, command );
	}
	
	void ResourceFileCopyingTask::Return( bool succeeded )
	{
	}
	
	static
	TaskPtr make_resources_task( const plus::string&  src,
	                             const plus::string&  dst )
	{
		TaskPtr resources_task( new NullTask() );
		
		n::owned< p7::dir_t > resources_dir = p7::opendir( src );
		
		errno = 0;
		
		while ( const dirent* ent = ::readdir( resources_dir ) )
		{
			const char* name = ent->d_name;
			
			if ( name[ 0 ] != '.' )
			{
				const plus::string src_file = src / name;
				const plus::string dst_file = dst / name;
				
				TaskPtr task( new ResourceFileCopyingTask( src_file, dst_file ) );
				
				UpdateInputStamp( task, src_file );
				
				resources_task->AddDependent( task );
			}
		}
		
		if ( errno != 0 )
		{
			p7::throw_errno( errno );
		}
		
		return resources_task;
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
	
	static const char* ar_name()
	{
		if ( const char* ar = getenv( "AR" ) )
		{
			return ar;
		}
		
		return "ar";
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
		
		link_command.push_back( ar_name() );
		link_command.push_back( "rcs"    );
		
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
			const char*         its_extension;
		
		public:
			object_filename_filler( const plus::string& objects, const char* extension )
			:
				its_objects_dir( objects ),
				its_extension( extension )
			{
				ASSERT( !its_objects_dir.empty() );
			}
			
			plus::string operator()( const plus::string& source_path ) const
			{
				const char* ext = its_extension;
				
				if ( ! ext )
				{
					const bool is_C = source_path.end()[ -2 ] == '.';
					
					ext = is_C ? ".i.o" : ".ii.o";
				}
				
				return derived_pathname( its_objects_dir, source_path, ext );
			}
	};
	
	static
	void FillObjectFiles( const plus::string&                 objects_dir,
	                      const std::vector< plus::string >&  source_paths,
	                      std::vector< plus::string >&        object_pathnames,
	                      const char*                         extension = ".o" )
	{
		std::transform( source_paths.begin(),
		                source_paths.end(),
		                object_pathnames.begin(),
		                object_filename_filler( objects_dir, extension ) );
	}
	
	static inline
	const char* dot_o( bool use_cpp )
	{
	#ifdef __RELIX__
		
		return use_cpp ? ".cpp.o" : ".o";
		
	#endif
		
		return use_cpp ? NULL : ".o";
	}
	
	void NameObjectFiles( const Project&                project,
	                      std::vector< plus::string >&  object_pathnames,
	                      bool                          use_cpp )
	{
		plus::string objects_dir = ProjectObjectsDirPath( project.Name() );
		
		const std::vector< plus::string >& sources = project.Sources();
		
		object_pathnames.resize( sources.size() );
		
		FillObjectFiles( objects_dir, sources, object_pathnames, dot_o( use_cpp ) );
	}
	
	
	void LinkProduct( Project&                       project,
	                  const TargetInfo&              targetInfo,
	                  const TaskPtr&                 project_base_task,
	                  const TaskPtr&                 source_dependency,
	                  const std::vector< TaskPtr >&  tool_dependencies )
	{
		const bool relix = TargetingRelix( targetInfo.envType == envRelix );
		
		const bool real_unix = !relix;  // unix is an evil macro on Linux
		
		plus::string diagnosticsDir = ProjectDiagnosticsDirPath( project.Name() );
		
		TaskPtr rmdir_diagnostics_task = seize_ptr( new RemoveDirTask( diagnosticsDir ) );
		
		std::vector< plus::string > objectFiles;
		
	#ifdef __RELIX__
		
		const bool preprocessing = Options().preprocess  &&  !get_project_providing_prefix( project, targetInfo.platform );
		
	#else
		
		// On non-MacRelix, precompiling does *not* override preprocessing
		const bool preprocessing = Options().preprocess;
		
	#endif
		
		NameObjectFiles( project, objectFiles, preprocessing );
		
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
			
		#if ALINE_RELIX_DEVELOPMENT
			
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
		
		AddProjectImports( project, targetInfo.platform, link_input_arguments );
		
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
			const size_t ext_len = strlen( dot_o( preprocessing ) );
			
			typedef std::vector< plus::string >::const_iterator Iter;
			
			const Iter end = objectFiles.begin() + n_tools;
			
			std::vector< TaskPtr >::const_iterator the_task = tool_dependencies.begin();
			
			for ( Iter the_object = objectFiles.begin();  the_object != end;  ++the_object, ++the_task )
			{
				const plus::string& objectFile = *the_object;
				
				plus::var_string linkOutput = outputDir / p7::basename( objectFile );
				
				// truncate ".o" or ".cpp.o"
				linkOutput.resize( linkOutput.size() - ext_len );
				
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
					
					const plus::string& proj_dir = project.ProjectFolder();
					
					plus::string info = project.ProjectFolder() / "Info.txt";
					
					if ( io::file_exists( info ) )
					{
						TaskPtr info_task( new InfoPListTask( project.Name(),
						                                      info,
						                                      contents ) );
						
						UpdateInputStamp( info_task, info );
						
						project_base_task->AddDependent( info_task );
						
						const char* Build = "Build-origin.txt";
						
						plus::string build = project.ProjectFolder() / Build;
						
						if ( io::file_exists( build ) )
						{
							UpdateInputStamp( info_task, build );
						}
					}
					
					exeDir = contents / "MacOS";
					
					pkginfo_dir = contents;
					
					const plus::string resources_src = proj_dir / "Resources";
					const plus::string resources_dst = contents / "Resources";
					
					if ( io::directory_exists( resources_src ) )
					{
						TaskPtr resources_task =
							make_resources_task( resources_src, resources_dst );
						
						project_base_task->AddDependent( resources_task );
					}
				}
				else if ( fileType != NULL )
				{
					pkginfo_dir = ProjectMetadataDirPath( project.Name() );
				}
				
				if ( fileType != NULL )
				{
					plus::string pkginfo = pkginfo_dir / "PkgInfo";
					
					WritePkgInfo( pkginfo.c_str(), fileType + creator );
					
					if ( relix )
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
				std::vector< plus::string > rsrc_pathnames;
				
				TaskPtr rez_task;
				
				const Platform carbonCFM = apiMacCarbon | runtimeCodeFragments;
				
				const bool needsCarbResource = product == productApplication  &&  (targetInfo.platform & carbonCFM) == carbonCFM;
				
				if ( needsCarbResource || !project.UsedRezFiles().empty() )
				{
					plus::string rez_output_pathname = RezzedDirPath() / project.Name() + ".rsrc";
					
					rsrc_pathnames.push_back( rez_output_pathname );
					
					rez_task = MakeRezTask( project, rez_output_pathname, needsCarbResource, relix );
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
					                                                         relix ) );
					
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
