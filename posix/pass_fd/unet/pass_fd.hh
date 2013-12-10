/*
	pass_fd.hh
	----------
*/

#ifndef UNET_PASSFD_HH
#define UNET_PASSFD_HH


namespace unet
{
	
	int send_fd( int socket_fd, int payload_fd );
	
	int recv_fd( int socket_fd );
	
}

#endif
