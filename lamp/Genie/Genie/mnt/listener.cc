/*
	listener.cc
	-----------
*/

#include "Genie/mnt/listener.hh"

// POSIX
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// posix-utils
#include "posix/listen_unix.hh"

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"

// vfs
#include "vfs/node_fwd.hh"

// relix-kernel
#include "relix/fs/console.hh"
#include "relix/task/scheduler.hh"

// freemount
#include "freemount/event_loop.hh"
#include "freemount/receiver.hh"
#include "freemount/server.hh"
#include "freemount/session.hh"

// Genie
#include "Genie/mnt/path.hh"
#include "Genie/mnt/root.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace Genie
{
	
	namespace mnt = freemount;
	
	using posix::listen_unix;
	
	
	struct wake_up_scope
	{
		wake_up_scope()
		{
			relix::pthread_wakeup();
			
			mac::sys::request_async_wakeup();
		}
	};
	
	class op_scope
	{
		typedef relix::synchronized_scope sync_scope;
		
		private:
			wake_up_scope  wake_up;
			sync_scope     sync;
	};
	
	struct client_data
	{
		int fd;
	};
	
	static
	int frame_handler( void* context, const mnt::frame_header& frame )
	{
		op_scope sync;
		
		return mnt::frame_handler( context, frame );
	}
	
	static
	void* client_proc( void* param )
	{
		client_data client = *(client_data*) param;
		
		delete (client_data*) param;
		
		const int remote_fd = client.fd;
		
		const vfs::node& root = freemount_root();
		
		mnt::session* s = NULL;
		
		try
		{
			{
				op_scope sync;
				
				// This manipulates shared refcounts.
				s = new mnt::session( remote_fd, root, root );
			}
			
			mnt::data_receiver r( &frame_handler, s );
			
			run_event_loop( r, remote_fd );
		}
		catch ( ... )
		{
			// out of memory?
		}
		
		close( remote_fd );
		
		op_scope sync;
		
		delete s;  // manipulates shared refcounts
		
		return NULL;
	}
	
	static
	void start_client( int remote_fd )
	{
		client_data* client = NULL;
		
		try
		{
			client = new client_data();
			
			client->fd = remote_fd;
		}
		catch ( ... )
		{
			close( remote_fd );
			
			return;
		}
		
		pthread_t client_thread;
		
		int err = pthread_create( &client_thread, NULL, &client_proc, client );
		
		if ( err )
		{
			close( remote_fd );
			
			delete client;
		}
		else
		{
			pthread_detach( client_thread );
		}
	}
	
	static
	void* listener_proc( void* param )
	{
		const int listener_fd = *(int*) param;
		
		for ( ;; )
		{
			sockaddr_storage peer;
			socklen_t len = sizeof peer;
			
			int remote_fd = accept( listener_fd, (sockaddr*) &peer, &len );
			
			if ( remote_fd >= 0 )
			{
				start_client( remote_fd );
			}
		}
	}
	
	static
	void socket_cleanup()
	{
		unlink( socket_path() );
	}
	
	static pthread_t listener_thread;
	
	static
	bool spawn_listener_service()
	{
		using relix::console::log;
		
		static int listener_fd = -1;
		
		if ( listener_fd >= 0 )
		{
			return false;
		}
		
		const char* path = socket_path();
		
		listener_fd = listen_unix( path );
		
		if ( listener_fd < 0 )
		{
			if ( errno == ENOENT )
			{
				log( STR_LEN(
					"Error: socket directory missing" "\n\n"
					"(Run `mkdir -p $HOME" FS_SUBPATH "` and relaunch.)" ) );
			}
			else
			{
				const char* error = strerror( errno );
				
				log( STR_LEN( "Error: couldn't create socket." "\n\n" ) );
				
				log( path, strlen( path ) );
				log( STR_LEN( ": " ) );
				log( error, strlen( error ) );
			}
			
			return false;
		}
		
		int* arg = &listener_fd;
		
		int err = pthread_create( &listener_thread, NULL, &listener_proc, arg );
		
		if ( err )
		{
			close( listener_fd );
			listener_fd = -1;
			
			socket_cleanup();
			
			return false;
		}
		
		atexit( &socket_cleanup );
		
		return true;
	}
	
	static
	void maintain_service_symlink()
	{
		const char* path = service_path();
		
		/*
			TODO:
				Try to connect to the service path.  If it works, close it
				and return.
				
				Otherwise, scan the ~/var/run/fs directory and try to connect
				to each socket.  Delete any that yield ECONNREFUSED.
				
				If our socket is the oldest (or it's tied for earliest mod
				date and has the lowest PID), update the symlink.
		*/
		
		unlink( path );
		
		symlink( socket_path(), path );
	}
	
	void start_gui_service()
	{
		spawn_listener_service();
		
		maintain_service_symlink();
	}
	
}
