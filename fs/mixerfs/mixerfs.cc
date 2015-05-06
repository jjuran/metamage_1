/*
	mixerfs.cc
	----------
*/

#include "mixerfs.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/extras/coprocess.hh"
#include "poseven/functions/close.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/vfork.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"


#define MIXCTL_PATH  "/opt/metamage/mixctl"


namespace mixerfs
{
	
	namespace p7 = poseven;
	
	
	static plus::string mixctl( const char* command, const char* arg = NULL )
	{
		// arg may be NULL
		
		int fds[2];
		
		p7::throw_posix_result( pipe( fds ) );
		
		p7::fd_t reader = p7::fd_t( fds[ 0 ] );
		p7::fd_t writer = p7::fd_t( fds[ 1 ] );
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			if ( ::dup2( writer, STDOUT_FILENO ) < 0 )
			{
				abort();
			}
			
			int null_fd = open( "/dev/null", O_RDWR );
			
			if ( null_fd < 0  ||  ::dup2( null_fd, STDERR_FILENO ) < 0 )
			{
				abort();
			}
			
			close( null_fd );
			
			close( fds[ 0 ] );
			close( fds[ 1 ] );
			
			const char* argv[] = { MIXCTL_PATH, command, arg, NULL };
			
			p7::execv( argv );
		}
		
		close( fds[ 1 ] );
		
		nucleus::owned< p7::fd_t > input = nucleus::owned< p7::fd_t >::seize( reader );
		
		p7::coprocess proc( pid );
		
		char buffer[ 64 ];
		
		ssize_t n_read = p7::read( input, buffer, sizeof buffer );
		
		p7::wait_t wait = proc.wait();
		
		if ( p7::wifsignaled( wait ) )
		{
			p7::throw_errno( EIO );
		}
		
		if ( p7::wexitstatus( wait ) != 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		if ( n_read != 0 )
		{
			--n_read;  // omit trailing LF
		}
		
		return plus::string( buffer, n_read );
	}
	
	struct route : vfs::readwrite_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			result = mixctl( "route" ).move();
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			plus::string arg( begin, end );
			
			mixctl( "route", arg.c_str() );
		}
	};
	
	struct volume : vfs::readwrite_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			result = mixctl( "volume" ).move();
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			plus::string arg( begin, end );
			
			mixctl( "volume", arg.c_str() );
		}
	};
	
	const vfs::fixed_mapping mappings[] =
	{
		{ "route",  &vfs::new_property, &vfs::property_params_factory< route  >::value },
		{ "volume", &vfs::new_property, &vfs::property_params_factory< volume >::value },
		
		{ NULL, NULL }
	};
	
}
