/*
	pass_fd.cc
	----------
*/

#include "unet/pass_fd.hh"

// STREAMS
#ifndef USE_STREAMS
#ifdef __RELIX__
#define USE_STREAMS 1
#else
#define USE_STREAMS 0
#endif
#endif
#if USE_STREAMS
#include <stropts.h>
#include <sys/ioctl.h>
#endif

// POSIX
#include <sys/socket.h>
#include <sys/uio.h>

// Standard C
#include <errno.h>


namespace unet
{
	
	const size_t control_len = CMSG_LEN( sizeof (int) );
	
	struct cmsg_fd_alloc
	{
		cmsghdr  header;
		int      _data;
	};
	
	
	int send_fd( int socket_fd, int payload_fd )
	{
	#if USE_STREAMS
		
		return ioctl( socket_fd, I_SENDFD, (int*) payload_fd );
		
	#endif
		
		iovec iov;
		
		iov.iov_base = (void*) "";
		iov.iov_len  = 1;
		
		cmsg_fd_alloc cmsg;
		
		cmsg.header.cmsg_level = SOL_SOCKET;
		cmsg.header.cmsg_type  = SCM_RIGHTS;
		cmsg.header.cmsg_len   = control_len;
		
		msghdr msg = { 0 };
		
		msg.msg_name    = 0;  // NULL
		msg.msg_namelen = 0;
		
		msg.msg_iov    = &iov;
		msg.msg_iovlen = 1;
		
		msg.msg_control    = &cmsg;
		msg.msg_controllen = control_len;
		
		*(int*) CMSG_DATA( &cmsg.header ) = payload_fd;
		
		if ( sendmsg( socket_fd, &msg, 0 ) != 1 )
		{
			return -1;
		}
		
		return 0;
	}
	
	int recv_fd( int socket_fd )
	{
	#if USE_STREAMS
		
		strrecvfd recvfd;
		
		int ioc = ioctl( socket_fd, I_RECVFD, (int*) &recvfd );
		
		return ioc < 0 ? ioc : recvfd.fd;
		
	#endif
		
		char buf[ 1 ];
		
		iovec iov;
		
		iov.iov_base = buf;
		iov.iov_len  = sizeof buf;
		
		cmsg_fd_alloc cmsg;
		
		msghdr msg = { 0 };
		
		msg.msg_name    = 0;  // NULL
		msg.msg_namelen = 0;
		
		msg.msg_iov    = &iov;
		msg.msg_iovlen = 1;
		
		msg.msg_control    = &cmsg;
		msg.msg_controllen = control_len;
		
		ssize_t n_bytes = recvmsg( socket_fd, &msg, 0 );
		
		if ( n_bytes < 0 )
		{
			return -1;
		}
		
		if ( n_bytes == 0 )
		{
			errno = ENOTCONN;
			
			return -1;
		}
		
		if ( buf[ 0 ] != '\0' )
		{
			errno = buf[ 0 ];
			
			return -1;
		}
		
		if ( msg.msg_controllen == control_len )
		{
			if ( +   cmsg.header.cmsg_level == SOL_SOCKET
			     &&  cmsg.header.cmsg_type  == SCM_RIGHTS
			     &&  cmsg.header.cmsg_len   == control_len )
			{
				int result_fd = *(int*) CMSG_DATA( &cmsg.header );
				
				return result_fd;
			}
		}
		
		errno = EINVAL;
		
		return -1;
	}
	
}
