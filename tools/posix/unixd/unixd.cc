/*
	unixd.cc
	--------
*/

// POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <string.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


static void spawn_server( int client, char** argv )
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		// We're the child
		dup2( client, STDIN_FILENO  );  // Input from client
		dup2( client, STDOUT_FILENO );  // Output to client
		//dup2( client, 2 );  // Error inherited
		
		int result = close( client );  // Extraneous fd
		
		execv( argv[ 0 ], argv );
		
		_exit( errno == ENOENT ? 127 : 126 );
	}
}

static void event_loop( int listener_fd, char** argv )
{
	while ( true )
	{
		sockaddr_un peer;
		socklen_t len = sizeof peer;
		
		int remote_fd = accept( listener_fd, (sockaddr*) &peer, &len );
		
		spawn_server( remote_fd, argv );
		
		close( remote_fd );
		
		int wait_status;
		
		wait( &wait_status );
	}
}

int main( int argc, char** argv )
{
	if ( argc <= 2 )
	{
		write( STDERR_FILENO, STR_LEN( "Usage: unixd <path> command\n" ) );
		
		return 1;
	}
	
	const char* path    = argv[ 1 ];
	const char* command = argv[ 2 ];
	
	write( STDOUT_FILENO, STR_LEN( "Daemon starting up..." ) );
	
	
	int listener_fd = socket( PF_LOCAL, SOCK_STREAM, 0 );
	
	const char* socket_path = "/tmp/unixd.sock";
	
	unlink( path );
	
	sockaddr_un un = { 0 };
	
	un.sun_family = AF_LOCAL;
	
	strcpy( un.sun_path, path );
	
	bind( listener_fd, (const sockaddr*) &un, sizeof un );
	
	listen( listener_fd, 5 );

	write( STDOUT_FILENO, STR_LEN( " done.\n" ) );
	
	event_loop( listener_fd, argv + 2 );
	
	close( listener_fd );
	
	return 0;
}
