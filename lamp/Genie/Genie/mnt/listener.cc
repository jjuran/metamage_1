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

// relix-kernel
#include "relix/fs/console.hh"

// Genie
#include "Genie/mnt/path.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace Genie
{
	
	using posix::listen_unix;
	
	
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
				write( remote_fd, STR_LEN( "THE SYSTEM IS DOWN\n" ) );
				close( remote_fd );
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
