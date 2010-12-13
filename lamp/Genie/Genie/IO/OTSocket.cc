/*	===========
 *	OTSocket.cc
 *	===========
 */

#include "Genie/IO/OTSocket.hh"

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C
#include <signal.h>

// POSIX
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/OpenTransport/Utilities/OTNotifier_Entrance.hh"

#include "Nitrogen/OpenTransport.hh"
#include "Nitrogen/OpenTransportProviders.hh"
#include "Nitrogen/OSUtils.hh"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/WakeUp.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/IO/SocketStream.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	class OTSocket : public SocketHandle
	{
		private:
			OpenTransportShare       itsOpenTransport;
			int                      itsBacklog;
			SocketAddress            itsSocketAddress;
			SocketAddress            itsPeerAddress;
		
		public:
			n::owned< EndpointRef >  itsEndpoint;
			OTResult                 its_result;
			SInt16                   n_incoming_connections;
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
			
			std::auto_ptr< IOHandle > Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const SocketAddress& GetSockName() const  { return itsSocketAddress; }
			const SocketAddress& GetPeerName() const  { return itsPeerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
	
	static pascal void socket_notifier( void*        context,
	                                    OTEventCode  code,
	                                    OTResult     result,
	                                    void*        cookie )
	{
		Ped::WakeUp();
		
		try
		{
			switch ( code )
			{
				case kOTSyncIdleEvent:
					// Hack to make sure we don't get starved for events
					Ped::AdjustSleepForTimer( 4 );
					
					yield();
					
					(void) check_signals( false );  // FIXME
					
					break;
				
				default:
					break;
			}
			
			if ( OTSocket* socket = (OTSocket*) context )
			{
				switch ( code )
				{
					case T_LISTEN:
						if ( TARGET_API_MAC_CARBON )
						{
							// Notifiers may be preempted in OS X
							::OTAtomicAdd16( 1, &socket->n_incoming_connections );
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
	
	static void SetUpEndpoint( EndpointRef endpoint, OTSocket* socket )
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( socket_notifier );
		
		// The new endpoint is synchronous and (by default) nonblocking.
		
		// The underlying endpoint is always nonblocking for send and recv
		// and blocking for connect and listen (until we add support)
		
		N::OTSetBlocking( endpoint );
		
		N::OTInstallNotifier( endpoint, gNotifyUPP, socket );
		
		N::OTUseSyncIdleEvents( endpoint, true );
	}
	
	OTSocket::OTSocket( bool nonblocking )
	:
		SocketHandle( nonblocking ),
		itsBacklog(),
		itsEndpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
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
		SetUpEndpoint( itsEndpoint, this );
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
		
		return (canRead ? kPollRead : 0) | kPollWrite;
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
				Mac::OTNotifier_Entrance entered( itsEndpoint );
				
				if ( it_has_received_FIN )
				{
					return 0;
				}
				
				err_count = ::OTCountDataBytes( itsEndpoint, &n_readable_bytes );
			}
			
			if ( err_count != kOTNoDataErr )
			{
				break;
			}
			
			// Hack to make sure we don't get starved for events
			Ped::AdjustSleepForTimer( 4 );
			
			try_again( IsNonblocking() );
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
			send_signal_to_current_process( SIGPIPE );
			
			p7::throw_errno( EPIPE );
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
			
			const ssize_t sent = N::OTSnd( itsEndpoint,
			                               data      + n_written,
			                               byteCount - n_written );
			
			if ( sent != kOTFlowErr )
			{
				N::ThrowOTResult( sent );
				
				n_written += sent;
			}
		}
		
		if ( IsNonblocking() )
		{
			if ( n_written == 0 )
			{
				p7::throw_errno( EAGAIN );
			}
		}
		else if ( n_written < byteCount )
		{
			yield();
			
			const bool signal_caught = check_signals( false );
			
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
			// Fail
		}
		
		itsSocketAddress.Assign( local, len );
	}
	
	void OTSocket::Listen( int backlog )
	{
		if ( itsSocketAddress.Get() == NULL )
		{
			p7::throw_errno( EDESTADDRREQ );
		}
		
		itsBacklog = backlog;
		
		// Throw out our tcp-only endpoint and make one with tilisten prepended
		itsEndpoint = N::OTOpenEndpoint( N::OTCreateConfiguration( "tilisten,tcp" ) );
		
		SetUpEndpoint( itsEndpoint, this );
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = reinterpret_cast< unsigned char* >( itsSocketAddress.Get() );
		reqAddr.addr.len = itsSocketAddress.Len();
		reqAddr.qlen = backlog;
		
		N::OTSetBlocking    ( itsEndpoint );
		N::OTSetAsynchronous( itsEndpoint );
		
		OTBind_sync( *this, &reqAddr );
		
		it_is_listener = true;
	}
	
	std::auto_ptr< IOHandle > OTSocket::Accept( sockaddr& client, socklen_t& len )
	{
		RepairListener();
		
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( &client );
		call.addr.maxlen = len;
		
		while ( n_incoming_connections == 0 )
		{
			try_again( IsNonblocking() );
		}
		
		::OTAtomicAdd16( -1, &n_incoming_connections );
		
		N::OTListen( itsEndpoint, &call );
		
		len = call.addr.len;
		
		OTSocket* handle = new OTSocket;
		
		std::auto_ptr< IOHandle > newSocket( handle );
		
		handle->itsPeerAddress.Assign( client, len );
		
		N::OTAccept( itsEndpoint, handle->itsEndpoint, &call );
		
		return newSocket;
	}
	
	void OTSocket::Connect( const sockaddr& server, socklen_t len )
	{
		N::OTSetAsynchronous( itsEndpoint );
		
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
	
	boost::shared_ptr< IOHandle > New_OT_Socket( bool nonblocking )
	{
		return seize_ptr( new OTSocket( nonblocking ) );
	}
	
}

