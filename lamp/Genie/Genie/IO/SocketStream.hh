/*	===============
 *	SocketStream.hh
 *	===============
 */

#ifndef GENIE_IO_SOCKET_HH
#define GENIE_IO_SOCKET_HH

// Standarc C++
#include <memory>

// POSIX
#include "errno.h"
#include <fcntl.h>
#include <sys/socket.h>

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	struct SocketAddress
	{
		sockaddr   address;
		socklen_t  length;
		
		SocketAddress()  {}
		SocketAddress( const sockaddr& addr, socklen_t len ) : address( addr ), length( len )  {}
		
		sockaddr const* Get() const  { return &address; }
		sockaddr      * Get()        { return &address; }
		
		socklen_t Len() const  { return length; }
		
		void Assign( const sockaddr& addr, socklen_t len )
		{
			address = addr;
			length  = len;
		}
	};
	
	class SocketHandle : public StreamHandle
	{
		public:
			SocketHandle( bool                               nonblocking,
			              const vfs::filehandle_method_set*  methods = NULL )
			:
				StreamHandle( nonblocking ? O_RDWR | O_NONBLOCK
				                          : O_RDWR,
				              methods )
			{
			}
			
			virtual ~SocketHandle()  {}
			
			bool IsSocket() const  { return true; }
	};
	
	template <> struct IOHandle_Downcast_Traits< SocketHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsSocket; }
		
		static int GetError( IOHandle& )  { return ENOTSOCK; }
	};
	
}

#endif

