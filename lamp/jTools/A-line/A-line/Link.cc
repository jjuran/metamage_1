/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C
#include <stdio.h>
#include <string.h>

// Standard C++
#include <algorithm>

// relix-long-filename-hash
#include "relix-long-filename-hash/hash_long_name.hh"

// posix-utils
#include "posix/update_file.hh"

// plus
#include "plus/string/concat.hh"
#include "plus/mac_utf8.hh"

// poseven
#include "poseven/functions/basename.hh"
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


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	bool avoid_long_names;
	
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
	
	
	static void AddProjectImports( const Project& project, Platform platform, StringVector& link_input_arguments )
	{
		const StringVector& used_project_names = project.AllUsedProjects();
		
		typedef StringVector::const_iterator Iter;
		
		for ( Iter the_name = used_project_names.begin();  the_name != used_project_names.end();  ++the_name )
		{
			const Project& used_project = GetProject( *the_name, platform );
			
			const StringVector& library_imports = used_project.LibImports();
			
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
	
	
	static void AddFrameworks( const StringVector& frameworkNames, StringVector& v )
	{
		typedef StringVector::const_iterator Iter;
		
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
	
	static void RemoveNonLibs( StringVector& usedProjects, Platform platform )
	{
		usedProjects.resize_down( std::remove_if( usedProjects.begin(),
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
	
	static inline
	void WritePkgInfo( const char* pathname, const plus::string& contents )
	{
		using posix::update_file;
		
		int nok = update_file( pathname, contents.data(), contents.size() );
		
		p7::throw_posix_result( nok );
	}
	
	
	static
	void AddLibraryLinkArgs( const StringVector& usedLibs, StringVector& v )
	{
		/*
			Link the libs in reverse order, so if foo depends on bar,
			foo will have precedence.  Somehow, this is actually required
			to actually link anything on Unix.
		*/
		
		for ( size_t i = usedLibs.size();  i > 0;  )
		{
			const plus::string& name = usedLibs[ --i ];
			
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
	
	void LinkingTask::Make()
	{
		plus::string output_filename = p7::basename( OutputPath() );
		
		ExecuteCommand( this, its_caption + output_filename, get_command(), get_diagnostics_file_path().c_str() );
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
			StringVector  itsInputPathnames;
			plus::string  itsIncludeDirPathname;
			bool          itIsTargetingRelix;
		
		public:
			RezzingTask( const StringVector&  input,
			             const plus::string&  output,
			             const plus::string&  includeDir,
			             bool                 relix )
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
	
	static const char out_of_order[] = "[68K-hosted Rez is out of order]";
	
	void RezzingTask::Make()
	{
	#ifdef __MC68K__
		
		if ( ! UpToDate() )
		{
			plus::string name = p7::basename( OutputPath() );
			
			fprintf( stderr, "SKIP  %s %s\n", name.c_str(), out_of_order );
		}
		
		return;
		
	#endif
		
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
			
			struct stat st;
			
			if ( stat( rez, &st ) != 0 )
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
			else
			{
				static plus::string rincludes = get_Interfaces_subdir( "RIncludes" );
				
				if ( stat( rincludes.c_str(), &st ) == 0 )
				
				rezCommand.back() = rincludes.c_str();
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
		
		ExecuteCommand( this, "REZ   " + p7::basename( OutputPath() ), rezCommand );
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
		const StringVector& input_filenames = project.UsedRezFiles();
		
		const size_t n = input_filenames.size();
		
		StringVector input_pathnames;
		
		// TODO:  Preallocate (n + needsCarbResource) slots
		
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
		
		TaskPtr rez_task = new RezzingTask( input_pathnames,
		                                    output_pathname,
		                                    includeDir,
		                                    relix );
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			UpdateInputStamp( rez_task, input_pathnames[ i ] );
		}

		return rez_task;
	}
	
	class ResourceCopyingTask : public FileTask
	{
		private:
			StringVector  itsInputPathnames;
			bool          itIsTargetingRelix;
		
		public:
			ResourceCopyingTask( const StringVector&  input,
			                     const plus::string&  output,
			                     bool                 relix )
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
		
		ExecuteCommand( this, "RSRC  " + p7::basename( OutputPath() ), command );
	}
	
	void ResourceCopyingTask::Return( bool succeeded )
	{
		// check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	class ResourceForkCleansingTask : public FileTask
	{
		public:
			ResourceForkCleansingTask( const plus::string& pathname )
			:
				FileTask( pathname )
			{
			}
			
			void Make();
	};
	
	void ResourceForkCleansingTask::Make()
	{
		Command command;
		
		command.push_back( "rcleanse" );
		
		if ( ! Options().verbose )
		{
			command.push_back( "-q" );  // quiet
		}
		
		command.push_back( OutputPath().c_str() );
		
		command.push_back( NULL );
		
		ExecuteCommand( this, "ZAP   " + p7::basename( OutputPath() ), command );
	}
	
	
	class FileCopyingTask : public FileTask
	{
		private:
			const plus::string its_input_pathname;
		
		public:
			FileCopyingTask( const plus::string&  input,
			                 const plus::string&  output )
			:
				FileTask          ( output ),
				its_input_pathname( input  )
			{
			}
			
			void Make();
			
			void Return( bool succeeded );
	};
	
	void FileCopyingTask::Make()
	{
		Command command;
		
		command.push_back( "cp"                       );
		command.push_back( its_input_pathname.c_str() );
		command.push_back( OutputPath()      .c_str() );
		command.push_back( NULL                       );
		
		const plus::string message = "COPY  " + p7::basename( OutputPath() );
		
		ExecuteCommand( this, message, command );
	}
	
	void FileCopyingTask::Return( bool succeeded )
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
				
				TaskPtr task( new FileCopyingTask( src_file, dst_file ) );
				
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
	
	
	static
	void make_task_depend_on_libs( const TaskPtr&       task,
	                               const StringVector&  used_project_names,
	                               Platform             platform )
	{
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = used_project_names.begin();  it != used_project_names.end();  ++it )
		{
			const plus::string& name = *it;
			
			const Project& project  = GetProject( name, platform );
			
			project.get_static_lib_task()->AddDependent( task );
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
		return new LinkingTask( link_command,
		                        output_path,
		                        begin,
		                        end,
		                        diagnostics_dir,
		                        caption );
	}
	
	template < class Container >
	static inline TaskPtr make_link_task( const Command&       link_command,
	                                      const plus::string&  output_path,
	                                      const Container&     input,
	                                      const plus::string&  diagnostics_dir,
	                                      const char*          caption = "LINK  " )
	{
		return new LinkingTask( link_command,
		                        output_path,
		                        input.begin(),
		                        input.end(),
		                        diagnostics_dir,
		                        caption );
	}
	
	template < class Iter >
	static TaskPtr MakeStaticLibTask( const plus::string&  output_pathname,
	                                  Iter                 begin,
	                                  Iter                 end,
	                                  const plus::string&  diagnostics_dir )
	{
		Command link_command;
		
		link_command.push_back( "lib-static" );
		link_command.push_back( "-o"         );
		
		return make_link_task( link_command,
		                       output_pathname,
		                       begin,
		                       end,
		                       diagnostics_dir,
		                       "LIB   " );
	}
	
	
	void FillOutputFiles( const plus::string&  output_dir,
	                      const StringVector&  source_paths,
	                      StringVector&        output_pathnames,
	                      const char*          ext_c,
	                      const char*          ext_cc )
	{
		for ( size_t i = 0;  i < source_paths.size();  ++i )
		{
			const plus::string& path = source_paths[ i ];
			
			const bool is_C = path.end()[ -2 ] == '.';
			
			const char* ext = is_C ? ext_c : ext_cc;
			
		#ifdef __APPLE__
			
			if ( avoid_long_names )
			{
				plus::var_string pathname = derived_pathname( output_dir,
				                                              path,
				                                              ext );
				
				size_t i = pathname.find_last_of( '/' ) + 1;
				size_t n = pathname.size() - i;
				
				plus::string filename( &pathname[ i ], n );
				
				plus::var_string hfs_filename = mac_from_utf8( filename );
				
				n = hfs_filename.size();
				
				if ( n > 31 )
				{
					unsigned char hashed[ 32 ];
					
					char* q = &hfs_filename[ 0 ];
					
					std::replace( q, q + n, ':', '/' );
					
					relix::hash_long_name( hashed, q, n );
					
					hfs_filename = hashed;
					
					q = &hfs_filename[ 0 ];
					
					std::replace( q, q + n, '/', ':' );
					
					filename = utf8_from_mac( hfs_filename );
					
					pathname.resize( i );
					pathname.append( filename );
				}
				
				output_pathnames.push_back( pathname );
				
				continue;
			}
			
		#endif
			
			output_pathnames.push_back( derived_pathname( output_dir,
			                                              path,
			                                              ext ) );
		}
	}
	
	static inline
	const char* dot_o( bool use_cpp )
	{
	#ifdef __RELIX__
		
		return use_cpp ? ".cpp.o" : ".o";
		
	#endif
		
		return use_cpp ? NULL : ".o";
	}
	
	void NameObjectFiles( const Project&  project,
	                      StringVector&   object_pathnames,
	                      bool            use_cpp )
	{
		const char* ext = dot_o( use_cpp );
		
		FillOutputFiles( ProjectObjectsDirPath( project.Name() ),
		                 project.Sources(),
		                 object_pathnames,
		                 ext ? ext : ".i.o",
		                 ext ? ext : ".ii.o" );
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
		
		TaskPtr rmdir_diagnostics_task = new RemoveDirTask( diagnosticsDir );
		
		StringVector objectFiles;
		
		const bool preprocessing = Options().preprocess  &&  !get_project_providing_prefix( project, targetInfo.platform );
		
		if ( can_run_Classic() )
		{
			avoid_long_names = targetInfo.toolchain == toolchainMetrowerks;
		}
		
		NameObjectFiles( project, objectFiles, preprocessing );
		
		const std::size_t n_tools = project.ToolCount();
		
		const ProductType product = project.Product();
		
		const bool exports_static_lib = product == productStaticLib  ||  product == productDropIn;
		
		const bool hasStaticLib = exports_static_lib  ||  product == productToolkit;
		
		plus::string libsDir = LibrariesDirPath();
		
		plus::string library_pathname;
		
		TaskPtr lib_task;
		
		CommandGenerator cmdgen( targetInfo );
		
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
		
		
		Command command;
		
		command.push_back( cmdgen.LinkerName() );
		
		AugmentCommand( command, cmdgen.TargetArchitecture() );
		
		const char* fileType = NULL;
		
		plus::string creator  = project.CreatorCode();
		
		const plus::string& progName = project.ProgramName();
		const plus::string& linkName = progName.empty() ? project.Name()
		                                                : progName;
		
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
			
			case productDA:
				fileType = "dfil";
				creator  = "MACS";
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "DRVR=0" ) );
				AugmentCommand( command, cmdgen.ResourceName     ( linkName.c_str() ) );
				break;
			
			case productDriver:
				fileType = "DRVR";
				creator  = "RSED";
				
				AugmentCommand( command, cmdgen.ResourceTypeAndID( "DRVR=0" ) );
				AugmentCommand( command, cmdgen.ResourceName     ( linkName.c_str() ) );
				break;
			
		#endif
			
			default:
				p7::write( p7::stderr_fileno, STR_LEN( "Sorry, this product can't be built on this platform.\n" ) );
				
				p7::throw_errno( EINVAL );
		}
		
	#if ALINE_MAC_DEVELOPMENT
		
		const plus::string& specified_creator = project.CreatorCode();
		
		if ( ! specified_creator.empty() )
		{
			creator = specified_creator;
		}
		else if ( creator.empty() )
		{
			creator = "\?\?\?\?";
		}
		
	#endif
		
		
		const bool toolkit = n_tools > 0;
		
		TaskPtr link_dependency_task = new NullTask();
		
		StringVector link_input_arguments;
		
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
		StringVector usedProjects = project.AllUsedProjects();
		
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
			
			if ( has_frameworks )
			{
				AddFrameworks( project.Frameworks(), link_input_arguments );
			}
		}
		
		
		if ( cmdgen.LinkerOptions()[0] )
		{
			command.push_back( cmdgen.LinkerOptions() );
		}
		
		if ( targetInfo.platform & model_near )
		{
			command.push_back( "-mpcrel" );
		}
		
		plus::string outputDir = ProjectOutputDirPath( project.Name() );
		
		command.push_back( "-o" );
		
		if ( toolkit )
		{
			const size_t ext_len = strlen( dot_o( preprocessing ) );
			
			typedef StringVector::const_iterator Iter;
			
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
			
			if ( product == productTool  &&  Options().install )
			{
				const plus::string& src_file = outFile;
				const plus::string  dst_file = InstallDirPath() / linkName;
				
				TaskPtr task( new FileCopyingTask( src_file, dst_file ) );
				
				UpdateInputStamp( task, src_file );
				
				link_task->AddDependent( task );
			}
			
			if ( ALINE_MAC_DEVELOPMENT )
			{
				StringVector rsrc_pathnames;
				
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
					rez_task = new NullTask();
				}
				
				project_base_task->AddDependent( rez_task );
				
				TaskPtr rcleanse_task;
				
				Task* rcleanse_prerequisite = NULL;
				
				if ( ! bundle )
				{
					rcleanse_task = new ResourceForkCleansingTask( outFile );
					
					rcleanse_prerequisite = link_task.get();
				}
				
				if ( !rsrc_pathnames.empty() )
				{
					plus::string rsrcFile = bundle ? outputDir / BundleResourceFileRelativePath( linkName )
					                               : outFile;
					
					TaskPtr copy_rsrcs = new ResourceCopyingTask( rsrc_pathnames,
					                                              rsrcFile,
					                                              relix );
					
					if ( bundle )
					{
						rez_task->AddDependent( copy_rsrcs );
					}
					else
					{
						rez_task->AddDependent( link_task );
						
						link_task->AddDependent( copy_rsrcs );
						
						rcleanse_prerequisite = copy_rsrcs.get();
					}
				}
				
				if ( rcleanse_prerequisite  &&  targetInfo.build != buildSymbolics )
				{
					rcleanse_prerequisite->AddDependent( rcleanse_task );
				}
			}
		}
	}
	
}
