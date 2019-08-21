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
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>

// Standard C
#include <errno.h>

// pass_fd
#include "unet/monitor_fdpass.hh"


namespace unet
{
	
	const size_t control_len = CMSG_LEN( sizeof (int) );
	
#if defined( __APPLE__ )  &&  ! defined( __clang__ )
	
	struct cmsg_fd_alloc
	{
		cmsghdr  header;
		int      _data;
	};
	
#else
	
	union cmsg_fd_alloc
	{
		cmsghdr  header;
		char     _alloc[ CMSG_SPACE( sizeof (int) ) ];
	};
	
#endif
	
	
	int send_fd( int socket_fd, int payload_fd )
	{
	#if USE_STREAMS
		
		return ioctl( socket_fd, I_SENDFD, (int*) payload_fd );
		
	#endif
		
	#if __OpenBSD__
		
		return openbsd::mm_send_fd( socket_fd, payload_fd );
		
	#endif
		
		iovec iov;
		
		iov.iov_base = (char*) "";
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
		
		msg.msg_control    = (char*) &cmsg;
		msg.msg_controllen = control_len;
		
		*(int*) CMSG_DATA( &cmsg.header ) = payload_fd;
		
		if ( sendmsg( socket_fd, &msg, 0 ) != 1 )
		{
			return -1;
		}
		
	#ifdef __APPLE__
		
		/*
			In Mac OS X 10.4.11 at least, two consecutive fd sends can fail
			if the first isn't received prior to the second.  To ensure
			this doesn't happen, block after sending until the receiver
			acknowledges.
			
			Turns out this was fixed in 10.5.  More info:
			<https://developer.apple.com/library/content/qa/qa1541/_index.html>
		*/
		
		char buffer;
		
		ssize_t n_read = read( socket_fd, &buffer, sizeof buffer );
		
		if ( n_read <= 0 )
		{
			if ( n_read == 0 )
			{
				errno = ENOTCONN;
			}
			
			return -1;
		}
		
	#endif
		
		return 0;
	}
	
	int recv_fd( int socket_fd )
	{
	#if USE_STREAMS
		
		strrecvfd recvfd;
		
		int ioc = ioctl( socket_fd, I_RECVFD, (int*) &recvfd );
		
		return ioc < 0 ? ioc : recvfd.fd;
		
	#endif
		
	#if __OpenBSD__
		
		return openbsd::mm_receive_fd( socket_fd );
		
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
		
		msg.msg_control    = (char*) &cmsg;
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
		
		if ( msg.msg_controllen >= control_len )
		{
			if ( +   cmsg.header.cmsg_level == SOL_SOCKET
			     &&  cmsg.header.cmsg_type  == SCM_RIGHTS
			     &&  cmsg.header.cmsg_len   == control_len )
			{
			#ifdef __APPLE__
				
				(void) write( socket_fd, "", 1 );
				
			#endif
				
				int result_fd = *(int*) CMSG_DATA( &cmsg.header );
				
				return result_fd;
			}
		}
		
		errno = EINVAL;
		
		return -1;
	}
	
}
