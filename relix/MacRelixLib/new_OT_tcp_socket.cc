/*
	new_OT_tcp_socket.cc
	--------------------
*/

#include "new_OT_tcp_socket.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// mac-config
#include "mac_config/open-transport.hh"
#include "mac_config/upp-macros.hh"

// poseven
#include "poseven/types/errno_t.hh"

#ifndef MAC_OS_X_VERSION_10_8

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"

// mac-app-utils
#include "mac_app/OpenTransport_share.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"

// Nitrogen
#include "Nitrogen/OpenTransport.hh"
#include "Nitrogen/OpenTransportProviders.hh"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"

// relix-kernel
#include "relix/api/broken_pipe.hh"
#include "relix/api/try_again.hh"
#include "relix/api/yield.hh"


namespace relix
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct OT_socket_extra
	{
		int backlog;
		
		InetAddress sock_addr;
		InetAddress peer_addr;
		
		EndpointRef endpoint;
		
		OTResult its_result;
		
		SInt32 n_incoming_connections;
		
		bool  it_is_bound;
		bool  it_is_listener;
		bool  it_is_connecting;
		bool  it_is_connected;
		bool  it_has_sent_FIN;
		bool  it_has_received_FIN;
		bool  it_has_received_RST;
	};
	
	static
	void destroy_OT_socket( vfs::filehandle* that )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( extra.endpoint )
		{
			::OTCloseProvider( extra.endpoint );
		}
		
		mac::app::CloseOpenTransport_shared();
	}
	
	
	static pascal void socket_notifier( void*        context,
	                                    OTEventCode  code,
	                                    OTResult     result,
	                                    void*        cookie )
	{
		mac::sys::request_async_wakeup();
		
		try
		{
			if ( OT_socket_extra* socket = (OT_socket_extra*) context )
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
						(void) ::OTRcvConnect( socket->endpoint, NULL );
						
						socket->it_is_connecting = false;
						socket->it_is_connected  = true;
						break;
					
					case T_DISCONNECT:
						(void) ::OTRcvDisconnect( socket->endpoint, NULL );
						
						socket->it_is_connecting    = false;
						socket->it_has_received_RST = true;
						break;
					
					case T_ORDREL:
						(void) ::OTRcvOrderlyDisconnect( socket->endpoint );
						
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
							socket->endpoint = (EndpointRef) cookie;
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
	
	static
	void OTBind_sync( OT_socket_extra&  socket,
	                  TBind*            reqAddr = NULL,
	                  TBind*            retAddr = NULL )
	{
		socket.its_result = 0;
		
		N::OTBind( socket.endpoint, reqAddr, retAddr );
		
		while ( socket.its_result == 0  &&  !socket.it_is_bound )
		{
			try_again( false );
		}
		
		N::ThrowOTResult( socket.its_result );
	}
	
	
	static
	void Complete( OT_socket_extra& socket )
	{
		while ( socket.its_result > 0  )
		{
			try_again( false );
		}
		
		N::ThrowOTResult( socket.its_result );
	}
	
	static
	void AsyncOpenEndpoint( const char* config, OT_socket_extra* socket )
	{
		DEFINE_UPP( OTNotify, socket_notifier )
		
		socket->its_result = 1;
		
		N::OTAsyncOpenEndpoint( N::OTCreateConfiguration( config ),
		                        UPP_ARG( socket_notifier ),
		                        socket );
		
		Complete( *socket );
	}
	
	
	static
	unsigned OT_poll( vfs::filehandle* that );
	
	static
	ssize_t OT_read( vfs::filehandle* that, char* buffer, size_t n );
	
	static
	ssize_t OT_write( vfs::filehandle* that, const char* buffer, size_t n );
	
	static
	void OT_bind( vfs::filehandle* that, const sockaddr* local, socklen_t len );
	
	static
	void OT_listen( vfs::filehandle* that, int backlog );
	
	static
	vfs::filehandle_ptr OT_accept( vfs::filehandle* that, sockaddr* client, socklen_t* len );
	
	static
	void OT_connect( vfs::filehandle* that, const sockaddr* server, socklen_t len );
	
	static
	void shut_down_writing( vfs::filehandle* that );
	
	static void OT_shutdown( vfs::filehandle* sock, int how )
	{
		if ( how != SHUT_WR )
		{
			// reading shutdown is unimplemented
		}
		
		if ( how != SHUT_RD )
		{
			shut_down_writing( sock );
		}
	}
	
	static const sockaddr* OT_getsockname( vfs::filehandle* sock )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) sock->extra();
		
		return (const sockaddr*) &extra.sock_addr;
	}
	
	static const sockaddr* OT_getpeername( vfs::filehandle* sock )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) sock->extra();
		
		return (const sockaddr*) &extra.peer_addr;
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
	
	vfs::filehandle_ptr new_OT_tcp_socket( bool nonblocking )
	{
		using namespace mac::app;
		
		InitOpenTransport_shared( &default_OSStatus_handler );
		
		const int flags = nonblocking ? O_RDWR | O_NONBLOCK
		                              : O_RDWR;
		
		vfs::filehandle* socket = new vfs::filehandle( flags,
		                                               &OT_methods,
		                                               sizeof (OT_socket_extra),
		                                               &destroy_OT_socket );
		
		OT_socket_extra& extra = *(OT_socket_extra*) socket->extra();
		
		memset( &extra, '\0', sizeof extra );
		
		vfs::filehandle_ptr result = socket;
		
		AsyncOpenEndpoint( "tcp", &extra );
		
		return result;
	}
	
	static
	bool repair_listener( vfs::filehandle* that )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( extra.it_is_listener  &&  ! OTGetEndpointState( extra.endpoint ) )
		{
			try_again( false );
			
			OT_listen( that, extra.backlog );
			
			return true;
		}
		
		return false;
	}
	
	static
	unsigned OT_poll( vfs::filehandle* that )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		repair_listener( that );
		
		::OTResult state = ::OTGetEndpointState( extra.endpoint );
		
		bool canRead = true;
		
		if ( state == T_IDLE )
		{
			canRead = false;
		}
		else if ( state == T_DATAXFER )
		{
			::OTByteCount count;
			
			canRead = ::OTCountDataBytes( extra.endpoint, &count ) == noErr;
		}
		
		return (canRead ? vfs::Poll_read : 0) | vfs::Poll_write;
	}
	
	static
	ssize_t OT_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( extra.it_has_received_FIN )
		{
			return 0;
		}
		
		::OTByteCount n_readable_bytes;
		
		OSStatus err_count;
		
		while ( true )
		{
			err_count = ::OTCountDataBytes( extra.endpoint, &n_readable_bytes );
			
			if ( extra.it_has_received_FIN )
			{
				return 0;
			}
			
			if ( err_count != kOTNoDataErr )
			{
				break;
			}
			
			try_again( is_nonblocking( *that ) );
		}
		
		Mac::ThrowOSStatus( err_count );
		
		if ( n > n_readable_bytes )
		{
			n = n_readable_bytes;
		}
		
		return N::OTRcv( extra.endpoint, buffer, n );
	}
	
	static
	ssize_t OT_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( extra.it_has_sent_FIN )
		{
			broken_pipe();
		}
		
		const char* p = buffer;
		
		std::size_t n_written = 0;
		
	retry:
		
		const ssize_t sent = ::OTSnd( extra.endpoint,
									  (char*) p + n_written,
									  n         - n_written,
									  0 );
		
		if ( extra.it_has_received_RST )
		{
			p7::throw_errno( ECONNRESET );
		}
		
		if ( sent != kOTFlowErr )
		{
			N::ThrowOTResult( sent );
			
			n_written += sent;
		}
		
		if ( is_nonblocking( *that ) )
		{
			if ( n_written == 0 )
			{
				p7::throw_errno( EAGAIN );
			}
		}
		else if ( n_written < n )
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
	
	static
	void OT_bind( vfs::filehandle* that, const sockaddr* local, socklen_t len )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( len != sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		extra.sock_addr = (const InetAddress&) *local;
	}
	
	static
	void OT_listen( vfs::filehandle* that, int backlog )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		extra.backlog = backlog;
		
		// Throw out our tcp-only endpoint and make one with tilisten prepended
		AsyncOpenEndpoint( "tilisten,tcp", &extra );
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = (uint8_t*) &extra.sock_addr;  // reinterpret_cast
		reqAddr.addr.len = sizeof extra.sock_addr;
		reqAddr.qlen = backlog;
		
		OTBind_sync( extra, &reqAddr );
		
		extra.it_is_listener = true;
	}
	
	static
	vfs::filehandle_ptr OT_accept( vfs::filehandle* that, sockaddr* client, socklen_t* len )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		repair_listener( that );
		
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		if ( *len < sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		call.addr.buf = reinterpret_cast< unsigned char* >( client );
		call.addr.maxlen = *len;
		
		while ( extra.n_incoming_connections == 0 )
		{
			try_again( is_nonblocking( *that ) );
		}
		
		::OTAtomicAdd32( -1, &extra.n_incoming_connections );
		
		N::OTListen( extra.endpoint, &call );
		
		*len = call.addr.len;
		
		if ( *len != sizeof (InetAddress) )
		{
			p7::throw_errno( EINVAL );
		}
		
		vfs::filehandle_ptr handle = new_OT_tcp_socket( false );
		
		OT_socket_extra& new_extra = *(OT_socket_extra*) handle->extra();
		
		new_extra.peer_addr = (const InetAddress&) *client;
		
		extra.its_result = 1;
		
		N::OTAccept( extra.endpoint, new_extra.endpoint, &call );
		
		Complete( extra );
		
		return handle;
	}
	
	static
	void OT_connect( vfs::filehandle* that, const sockaddr* server, socklen_t len )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		if ( ! extra.it_is_bound )
		{
			OTBind_sync( extra );
		}
		
		TCall sndCall;
		
		::OTMemzero( &sndCall, sizeof (TCall) );
		
		sndCall.addr.buf = reinterpret_cast< unsigned char* >( const_cast< sockaddr* >( server ) );
		sndCall.addr.len = len;
		
		extra.it_is_connecting = true;
		
		OSStatus err = ::OTConnect( extra.endpoint, &sndCall, NULL );
		
		if ( err != kOTNoDataErr )
		{
			extra.it_is_connecting = false;
			
			Mac::ThrowOSStatus( err );
		}
		
		while ( extra.it_is_connecting )
		{
			try_again( false );
		}
		
		if ( extra.it_has_received_RST )
		{
			// FIXME:  We should check the discon info, but for now assume
			p7::throw_errno( ECONNREFUSED );
		}
	}
	
	static
	void shut_down_writing( vfs::filehandle* that )
	{
		OT_socket_extra& extra = *(OT_socket_extra*) that->extra();
		
		N::OTSndOrderlyDisconnect( extra.endpoint );
		
		extra.it_has_sent_FIN = true;
	}
	
}

#else

int dummy;

#endif  // #ifndef MAC_OS_X_VERSION_10_8
