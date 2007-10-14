/*	===============
 *	SocketStream.hh
 *	===============
 */

#ifndef GENIE_IO_SOCKET_HH
#define GENIE_IO_SOCKET_HH

// Standarc C++
#include <memory>

// POSIX
#include <sys/socket.h>

// Genie
#include "Genie/IO/Stream.hh"


struct InetAddress;  // FIXME

namespace Genie
{
	
	typedef const InetAddress* ConstSockAddrParam;
	typedef       InetAddress*      SockAddrParam;
	
	struct SocketAddress;
	
	class SocketHandle : public StreamHandle
	{
		public:
			virtual ~SocketHandle()  {}
			
			static TypeCode Type()  { return kSocketType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			//void IOCtl( unsigned long request, int* argp );
			
			virtual void Bind( ConstSockAddrParam local, socklen_t len ) = 0;
			
			virtual void Listen( int backlog ) = 0;
			
			virtual std::auto_ptr< IOHandle > Accept( SockAddrParam client, socklen_t& len ) = 0;
			
			virtual void Connect( ConstSockAddrParam server, socklen_t len ) = 0;
			
			virtual const SocketAddress& GetSockName() const = 0;
			virtual const SocketAddress& GetPeerName() const = 0;
			
			virtual void ShutdownReading() = 0;
			virtual void ShutdownWriting() = 0;
	};
	
}

#endif

