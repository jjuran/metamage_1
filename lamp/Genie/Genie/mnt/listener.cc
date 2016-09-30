/*
	listener.cc
	-----------
*/

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

// relix-kernel
#include "relix/fs/console.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/mnt/path.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace Genie
{
	
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
	void* client_proc( void* param )
	{
		client_data client = *(client_data*) param;
		
		delete (client_data*) param;
		
		const int remote_fd = client.fd;
		
		char buffer[ 256 ];
		ssize_t n_read;
		
		try
		{
			while ( (n_read = read( remote_fd, buffer, sizeof buffer )) > 0 )
			{
				op_scope sync;
				
				relix::console::log( buffer, n_read );
			}
		}
		catch ( ... )
		{
			// out of memory?
		}
		
		close( remote_fd );
		
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
		
		log( STR_LEN( "Listening on " ) );
		log( path, strlen( path ) );
		
		return true;
	}
	
	struct listener_startup
	{
		listener_startup()
		{
			spawn_listener_service();
		}
	};
	
#ifndef __RELIX__
	
	listener_startup listen_at_startup;
	
#endif
	
}
