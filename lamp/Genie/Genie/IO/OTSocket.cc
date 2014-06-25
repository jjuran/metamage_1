/*	===========
 *	OTSocket.cc
 *	===========
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/new_tcp_socket.hh"

#ifndef MAC_OS_X_VERSION_10_8

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C
#include <signal.h>

// POSIX
#include <sys/socket.h>

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"

// Nitrogen
#include "Mac/OpenTransport/Utilities/OTNotifier_Entrance.hh"

#include "Nitrogen/OpenTransport.hh"
#include "Nitrogen/OpenTransportProviders.hh"
#include "Nitrogen/OSUtils.hh"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct InetAddress : private ::InetAddress
	{
		operator const sockaddr&() const  { return *(const sockaddr*) this; }
	};
	
	class OTSocket : public vfs::filehandle
	{
		private:
			OpenTransportShare       itsOpenTransport;
			int                      itsBacklog;
			InetAddress              itsSocketAddress;
			InetAddress              itsPeerAddress;
		
		public:
			n::owned< EndpointRef >  itsEndpoint;
			OTResult                 its_result;
			SInt32                   n_incoming_connections;
			bool                     it_is_bound;
			bool                     it_is_listener;
			bool                     it_is_connecting;
			bool                     it_is_connected;
			bool                     it_has_sent_FIN;
			bool                     it_has_received_FIN;
			bool                     it_has_received_RST;
		
		public:
			OTSocket( bool nonblocking = false );
			
			~OTSocket();
			
			bool RepairListener();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			void Bind( const sockaddr& local, socklen_t len );
			
			void Listen( int backlog );
			
			vfs::filehandle_ptr Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const sockaddr& GetSockName() const  { return itsSocketAddress; }
			const sockaddr& GetPeerName() const  { return itsPeerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
	
	static pascal void socket_notifier( void*        context,
	                                    OTEventCode  code,
	                                    OTResult     result,
	                                    void*        cookie )
	{
		mac::sys::request_async_wakeup();
		
		try
		{
			if ( OTSocket* socket = (OTSocket*) context )
			{
				switch ( code )
				{
					case T_LISTEN:
						if ( TARGET_API_MAC_CARBON )
						{
							// Notifiers may be preempted in OS X
							::OTAtomicAdd32( 1, &socket->n_incoming_connections );
						}
						else
						{
							++socket->n_incoming_connections;
						}
						break;
					
					case T_CONNECT:
						(void) ::OTRcvConnect( socket->itsEndpoint, NULL );
						
						socket->it_is_connecting = false;
						socket->it_is_connected  = true;
						break;
					
					case T_DISCONNECT:
						(void) ::OTRcvDisconnect( socket->itsEndpoint, NULL );
						
						socket->it_is_connecting    = false;
						socket->it_has_received_RST = true;
						break;
					
					case T_ORDREL:
						(void) ::OTRcvOrderlyDisconnect( socket->itsEndpoint );
						
						socket->it_has_received_FIN = true;
						
						break;
					
					case T_BINDCOMPLETE:
						socket->its_result = result;
						
						if ( result == noErr )
						{
							socket->it_is_bound = true;
						}
						
						break;
					
					case T_ACCEPTCOMPLETE:
						socket->its_result = result;
						
						break;
					
					case T_OPENCOMPLETE:
						socket->its_result = result;
						
						if ( result == noErr )
						{
							socket->itsEndpoint = n::owned< EndpointRef >::seize( (EndpointRef) cookie );
						}
						
						break;
					
					default:
						break;
				}
			}
		}
		catch ( ... )
		{
		}
	}
	
	static void OTBind_sync( OTSocket&  socket,
	                         TBind*     reqAddr = NULL,
	                         TBind*     retAddr = NULL )
	{
		socket.its_result = 0;
		
		N::OTBind( socket.itsEndpoint, reqAddr, retAddr );
		
		while ( socket.its_result == 0  &&  !socket.it_is_bound )
		{
			try_again( false );
		}
		
		N::ThrowOTResult( socket.its_result );
	}
	
	
	static OTNotifyUPP gSocketNotifier = ::NewOTNotifyUPP( socket_notifier );
	
	
	static void Complete( OTSocket& socket )
	{
		while ( socket.its_result > 0  )
		{
			try_again( false );
		}
		
		N::ThrowOTResult( socket.its_result );
	}
	
	static void AsyncOpenEndpoint( const char* config, OTSocket* socket )
	{
		socket->its_result = 1;
		
		N::OTAsyncOpenEndpoint( N::OTCreateConfiguration( config ),
		                        gSocketNotifier,
		                        socket );
		
		Complete( *socket );
	}
	
	
	static unsigned OT_poll( vfs::filehandle* sock )
	{
		return static_cast< OTSocket& >( *sock ).SysPoll();
	}
	
	static ssize_t OT_read( vfs::filehandle* sock, char* buffer, size_t n )
	{
		return static_cast< OTSocket& >( *sock ).SysRead( buffer, n );
	}
	
	static ssize_t OT_write( vfs::filehandle* sock, const char* buffer, size_t n )
	{
		return static_cast< OTSocket& >( *sock ).SysWrite( buffer, n );
	}
	
	static void OT_bind( vfs::filehandle* sock, const sockaddr* local, socklen_t len )
	{
		static_cast< OTSocket& >( *sock ).Bind( *local, len );
	}
	
	static void OT_listen( vfs::filehandle* sock, int backlog )
	{
		static_cast< OTSocket& >( *sock ).Listen( backlog );
	}
	
	static vfs::filehandle_ptr OT_accept( vfs::filehandle* sock, sockaddr* client, socklen_t* len )
	{
		return static_cast< OTSocket& >( *sock ).Accept( *client, *len );
	}
	
	static void OT_connect( vfs::filehandle* sock, const sockaddr* server, socklen_t len )
	{
		static_cast< OTSocket& >( *sock ).Connect( *server, len );
	}
	
	static void OT_shutdown( vfs::filehandle* sock, int how )
	{
		if ( how != SHUT_WR )
		{
			static_cast< OTSocket& >( *sock ).ShutdownReading();
		}
		
		if ( how != SHUT_RD )
		{
			static_cast< OTSocket& >( *sock ).ShutdownWriting();
		}
	}
	
	static const sockaddr* OT_getsockname( vfs::filehandle* sock )
	{
		return &static_cast< OTSocket& >( *sock ).GetSockName();
	}
	
	static const sockaddr* OT_getpeername( vfs::filehandle* sock )
	{
		return &static_cast< OTSocket& >( *sock ).GetPeerName();
	}
	
	static const vfs::stream_method_set OT_stream_methods =
	{
		&OT_poll,
		&OT_read,
		&OT_write,
	};
	
	static const vfs::socket_method_set OT_socket_methods =
	{
		&OT_bind,
		&OT_listen,
		&OT_accept,
		&OT_connect,
		&OT_shutdown,
		&OT_getsockname,
		&OT_getpeername,
	};
	
	static const vfs::filehandle_method_set OT_methods =
	{
		NULL,
		&OT_socket_methods,
		&OT_stream_methods,
	};
	
	OTSocket::OTSocket( bool nonblocking )
	:
		vfs::filehandle( nonblocking ? O_RDWR | O_NONBLOCK
		                             : O_RDWR,
		              &OT_methods ),
		itsBacklog(),
		its_result            ( 0 ),
		n_incoming_connections( 0 ),
		it_is_bound        ( false ),
		it_is_listener     ( false ),
		it_is_connecting   ( false ),
		it_is_connected    ( false ),
		it_has_sent_FIN    ( false ),
		it_has_received_FIN( false ),
		it_has_received_RST( false )
	{
		AsyncOpenEndpoint( "tcp", this );
	}
	
	OTSocket::~OTSocket()
	{
	}
	
	bool OTSocket::RepairListener()
	{
		if ( it_is_listener && ::OTGetEndpointState( itsEndpoint ) == 0 )
		{
			try_again( false );
			
			Listen( itsBacklog );
			
			return true;
		}
		
		return false;
	}
	
	unsigned int OTSocket::SysPoll()
	{
		RepairListener();
		
		::OTResult state = ::OTGetEndpointState( itsEndpoint );
		
		bool canRead = true;
		
		if ( state == T_IDLE )
		{
			canRead = false;
		}
		else if ( state == T_DATAXFER )
		{
			::OTByteCount count;
			
			canRead = ::OTCountDataBytes( itsEndpoint, &count ) == noErr;
		}
		
		return (canRead ? vfs::Poll_read : 0) | vfs::Poll_write;
	}
	
	ssize_t OTSocket::SysRead( char* data, std::size_t byteCount )
	{
		if ( it_has_received_FIN )
		{
			return 0;
		}
		
		::OTByteCount n_readable_bytes;
		
		OSStatus err_count;
		
		while ( true )
		{
			{
				err_count = ::OTCountDataBytes( itsEndpoint, &n_readable_bytes );
				
				if ( it_has_received_FIN )
				{
					return 0;
				}
			}
			
			if ( err_count != kOTNoDataErr )
			{
				break;
			}
			
			try_again( is_nonblocking( *this ) );
		}
		
		Mac::ThrowOSStatus( err_count );
		
		if ( byteCount > n_readable_bytes )
		{
			byteCount = n_readable_bytes;
		}
		
		return N::OTRcv( itsEndpoint, data, byteCount );
	}
	
	ssize_t OTSocket::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( it_has_sent_FIN )
		{
			broken_pipe();
		}
		
		const char* p = data;
		
		std::size_t n_written = 0;
		
	retry:
		
		{
			Mac::OTNotifier_Entrance entered( itsEndpoint );
			
			if ( it_has_received_RST )
			{
				p7::throw_errno( ECONNRESET );
			}
			
			const ssize_t sent = ::OTSnd( itsEndpoint,
			                              (char*) p + n_written,
			                              byteCount - n_written,
			                              0 );
			
			if ( sent != kOTFlowErr )
			{
				N::ThrowOTResult( sent );
				
				n_written += sent;
			}
		}
		
		if ( is_nonblocking( *this ) )
		{
			if ( n_written == 0 )
			{
				p7::throw_errno( EAGAIN );
			}
		}
		else if ( n_written < byteCount )
		{
			const bool signal_caught = yield( false );
			
			if ( !signal_caught )
			{
				goto retry;
			}
			
			// Some bytes were written when a signal was caught
		}
		
		return n_written;
	}
	
	void OTSocket::Bind( const sockaddr& local, socklen_t len )
	{
		if ( len != sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		itsSocketAddress = (const InetAddress&) local;
	}
	
	void OTSocket::Listen( int backlog )
	{
		itsBacklog = backlog;
		
		// Throw out our tcp-only endpoint and make one with tilisten prepended
		AsyncOpenEndpoint( "tilisten,tcp", this );
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = (unsigned char*) &itsSocketAddress;  // reinterpret_cast
		reqAddr.addr.len = sizeof itsSocketAddress;
		reqAddr.qlen = backlog;
		
		OTBind_sync( *this, &reqAddr );
		
		it_is_listener = true;
	}
	
	vfs::filehandle_ptr OTSocket::Accept( sockaddr& client, socklen_t& len )
	{
		RepairListener();
		
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		if ( len < sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		call.addr.buf = reinterpret_cast< unsigned char* >( &client );
		call.addr.maxlen = len;
		
		while ( n_incoming_connections == 0 )
		{
			try_again( is_nonblocking( *this ) );
		}
		
		::OTAtomicAdd32( -1, &n_incoming_connections );
		
		N::OTListen( itsEndpoint, &call );
		
		len = call.addr.len;
		
		if ( len != sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		OTSocket* handle = new OTSocket;
		
		vfs::filehandle_ptr newSocket( handle );
		
		handle->itsPeerAddress = (const InetAddress&) client;
		
		its_result = 1;
		
		N::OTAccept( itsEndpoint, handle->itsEndpoint, &call );
		
		Complete( *this );
		
		return newSocket;
	}
	
	void OTSocket::Connect( const sockaddr& server, socklen_t len )
	{
		if ( !it_is_bound )
		{
			OTBind_sync( *this );
		}
		
		TCall sndCall;
		
		::OTMemzero( &sndCall, sizeof (TCall) );
		
		sndCall.addr.buf = reinterpret_cast< unsigned char* >( const_cast< sockaddr* >( &server ) );
		sndCall.addr.len = len;
		
		it_is_connecting = true;
		
		OSStatus err = ::OTConnect( itsEndpoint, &sndCall, NULL );
		
		if ( err != kOTNoDataErr )
		{
			it_is_connecting = false;
			
			Mac::ThrowOSStatus( err );
		}
		
		while ( it_is_connecting )
		{
			try_again( false );
		}
		
		if ( it_has_received_RST )
		{
			// FIXME:  We should check the discon info, but for now assume
			p7::throw_errno( ECONNREFUSED );
		}
	}
	
	void OTSocket::ShutdownWriting()
	{
		N::OTSndOrderlyDisconnect( itsEndpoint );
		
		it_has_sent_FIN = true;
	}
	
}

#endif  // #ifndef MAC_OS_X_VERSION_10_8

namespace relix
{
	
	vfs::filehandle_ptr new_tcp_socket( bool nonblocking )
	{
	#ifndef MAC_OS_X_VERSION_10_8
		
		return new Genie::OTSocket( nonblocking );
		
	#endif
		
		return NULL;
	}
	
}

