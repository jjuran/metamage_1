/*
	paired_socket.cc
	----------------
*/

#include "relix/socket/paired_socket.hh"

// STREAMS
#ifdef __RELIX__
#include <stropts.h>
#endif

// POSIX
#include <fcntl.h>
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"
#include "vfs/filehandle/primitives/conveying.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/broken_pipe.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/api/try_again.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	static unsigned pairedsocket_poll( vfs::filehandle* that )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		return   vfs::Poll_read  * extra.input ->is_readable()
		       | vfs::Poll_write * extra.output->is_writable();
	}
	
	static ssize_t pairedsocket_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		return extra.input->read( buffer, n, is_nonblocking( *that ), &try_again );
	}
	
	static ssize_t pairedsocket_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		return extra.output->write( buffer, n, is_nonblocking( *that ), &try_again, &broken_pipe );
	}
	
	void pairedsocket_shutdown( vfs::filehandle* that, int how )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		if ( how != SHUT_WR )
		{
			extra.input->close_egress();
		}
		
		if ( how != SHUT_RD )
		{
			extra.output->close_ingress();
		}
	}
	
	int pairedsocket_conveying( vfs::filehandle* that )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		return !extra.input->empty();
	}
	
	static vfs::filehandle& check_fh( vfs::filehandle& fh )
	{
		if ( conveying( fh ) )
		{
			// To prevent cycles, don't send a socket with fds in flight
			
			p7::throw_errno( ELOOP );
		}
		
		return fh;
	}
	
	static void pairedsocket_ioctl( vfs::filehandle* that, unsigned long request, int* argp )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		switch ( request )
		{
		#ifdef __RELIX__
			
			case I_SENDFD:
				// FIXME:  Implement garbage collection to allow cycles
				
				extra.output->send_fd( check_fh( get_fd_handle( (int) argp ) ) );
				
				break;
			
			case I_RECVFD:
				if ( argp != NULL )
				{
					strrecvfd* arg = (strrecvfd*) argp;
					
					int fd = first_free_fd();
					
					assign_fd( fd, *extra.input->recv_fd( is_nonblocking( *that ), &try_again ) );
					
					arg->fd = fd;
					
					arg->uid = 0;
					arg->gid = 0;
				}
				
				break;
			
		#endif
			
			default:
				p7::throw_errno( EINVAL );
				
				break;
		}
	}
	
	
	const vfs::stream_method_set pairedsocket_stream_methods =
	{
		&pairedsocket_poll,
		&pairedsocket_read,
		&pairedsocket_write,
	};
	
	static const vfs::socket_method_set pairedsocket_socket_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&pairedsocket_shutdown,
		NULL,
		NULL,
		&pairedsocket_conveying,
	};
	
	const vfs::general_method_set pairedsocket_general_methods =
	{
		NULL,
		&pairedsocket_ioctl,
	};
	
	const vfs::filehandle_method_set pairedsocket_methods =
	{
		NULL,
		&pairedsocket_socket_methods,
		&pairedsocket_stream_methods,
		&pairedsocket_general_methods,
	};
	
	void close_paired_socket( vfs::filehandle* that )
	{
		pairedsocket_extra& extra = *(pairedsocket_extra*) that->extra();
		
		extra.input ->close_egress();
		extra.output->close_ingress();
		
		intrusive_ptr_release( extra.input  );
		intrusive_ptr_release( extra.output );
	}
	
	
	vfs::filehandle_ptr new_paired_socket( vfs::stream&  input,
	                                       vfs::stream&  output,
	                                       bool          nonblocking )
	{
		vfs::filehandle* result = new vfs::filehandle( NULL,
		                                               nonblocking ? O_RDWR | O_NONBLOCK : O_RDWR,
		                                               &pairedsocket_methods,
		                                               sizeof (pairedsocket_extra),
		                                               &close_paired_socket );
		
		pairedsocket_extra& extra = *(pairedsocket_extra*) result->extra();
		
		intrusive_ptr_add_ref( extra.input  = &input  );
		intrusive_ptr_add_ref( extra.output = &output );
		
		return result;
	}
	
}
