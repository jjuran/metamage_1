/*
	uloop.cc
	--------
*/

#include "unet/pass_fd.hh"

// POSIX
#include <sys/socket.h>


#define CHECK_N( call )  if ( (call) < 0 ) return 1; else


int main( int argc, char** argv )
{
	const int socket_fd = 1;  // STDOUT_FILENO
	
	int fds[ 2 ];
	
	CHECK_N( socketpair( PF_LOCAL, SOCK_STREAM, 0, fds ) );
	
	CHECK_N( unet::send_fd( socket_fd, fds[ 0 ] ) );
	CHECK_N( unet::send_fd( socket_fd, fds[ 1 ] ) );
	
	return 0;
}
