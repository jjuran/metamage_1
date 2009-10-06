/*	=====
 *	rm.cc
 *	=====
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Iota
#include "iota/strings.hh"

// Io
#include "io/walk.hh"

// poseven
#include "poseven/Directory.hh"
#include "poseven/FileDescriptor.hh"
#include "poseven/Pathnames.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static bool globally_forced = false;
	
	static p7::exit_t global_exit_status = p7::exit_success;
	
	
	static void delete_file( const char* path )
	{
		int unlinked = ::unlink( path );
		
		if ( unlinked < 0  &&  !(globally_forced && errno == ENOENT) )
		{
			std::fprintf( stderr, "rm: %s: %s\n", path, std::strerror( errno ) );
			
			global_exit_status = p7::exit_failure;
		}
	}
	
	
	struct file_deleter
	{
		void operator()( const std::string& path, unsigned depth ) const
		{
			delete_file( path.c_str() );
		}
	};
	
	
	static void recursive_delete( const char* path )
	{
		if ( globally_forced && !io::item_exists( path ) )
		{
			return;
		}
		
		recursively_walk_tree( std::string( path ),
		                       io::walk_noop(),
		                       file_deleter(),
		                       io::directory_deleter< std::string >() );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool recursive = false;
		
		o::bind_option_to_variable( "-r", recursive       );
		o::bind_option_to_variable( "-f", globally_forced );
		
		o::alias_option( "-r", "-R" );
		o::alias_option( "-r", "--recursive" );
		o::alias_option( "-f", "--force" );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		// Check for sufficient number of args
		if ( n_args < 1 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "rm: missing arguments\n" ) );
			
			return p7::exit_failure;
		}
		
		typedef void (*deleter_f)(const char*);
		
		deleter_f deleter = recursive ? recursive_delete
		                              : delete_file;
		
		for ( int index = 0;  index < n_args;  ++index )
		{
			const char* path = free_args[ index ];
			
			deleter( path );
		}
		
		return global_exit_status;
	}
	
}

