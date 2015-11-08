/*
	monitor_fdpass.hh
	-----------------
*/

#ifndef UNET_MONITORFDPASS_HH
#define UNET_MONITORFDPASS_HH


namespace openbsd
{
	
	int mm_send_fd( int socket_fd, int payload_fd );
	
	int mm_receive_fd( int socket_fd );
	
}

#endif
