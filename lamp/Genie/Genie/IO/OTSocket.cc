/*	===========
 *	OTSocket.cc
 *	===========
 */

#include "Genie/IO/OTSocket.hh"

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// Standard C/C++
#include <cstdio>

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
			bool                     itIsBound;
			bool                     itIsListener;
			bool                     itHasSentFIN;
			bool                     itHasReceivedFIN;
			bool                     itHasReceivedRST;
		
		public:
			OTSocket( bool nonblocking = false );
			
			~OTSocket();
			
			void ReceiveDisconnect();
			
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
					case T_ORDREL:
						(void) ::OTRcvOrderlyDisconnect( socket->itsEndpoint );
						
						socket->itHasReceivedFIN = true;
						
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
		itIsBound       ( false ),
		itIsListener    ( false ),
		itHasSentFIN    ( false ),
		itHasReceivedFIN( false ),
		itHasReceivedRST( false )
	{
		SetUpEndpoint( itsEndpoint, this );
	}
	
	OTSocket::~OTSocket()
	{
	}
	
	void OTSocket::ReceiveDisconnect()
	{
		N::OTRcvDisconnect( itsEndpoint );
		
		itHasReceivedRST = true;
	}
	
	bool OTSocket::RepairListener()
	{
		if ( itIsListener && ::OTGetEndpointState( itsEndpoint ) == 0 )
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
		if ( itHasReceivedFIN )
		{
			return 0;
		}
		
		::OTByteCount n_readable_bytes;
		
		OSStatus err_count;
		
		while ( true )
		{
			{
				Mac::OTNotifier_Entrance entered( itsEndpoint );
				
				if ( itHasReceivedFIN )
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
		
		N::ThrowOSStatus( err_count );
		
		if ( byteCount > n_readable_bytes )
		{
			byteCount = n_readable_bytes;
		}
		
		try
		{
			return N::OTRcv( itsEndpoint, data, byteCount );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == kOTLookErr )
			{
				OTResult look = N::OTLook( itsEndpoint );
				
				std::fprintf( stderr, "OTResult %d from OTLook()\n", look );
			}
			else
			{
				std::fprintf( stderr, "OSStatus %d from OTRcv()\n", err.Get() );
			}
			
			throw;
		}
		
		// Not reached, but MWCPPC 2.4.1 doesn't realize that
		return 0;
	}
	
	ssize_t OTSocket::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( itHasSentFIN )
		{
			send_signal_to_current_process( SIGPIPE );
			
			p7::throw_errno( EPIPE );
		}
		
		const char* p = data;
		
		std::size_t n_written = 0;
		
	retry:
		
		try
		{
			const ssize_t sent = N::OTSnd( itsEndpoint,
			                               data      + n_written,
			                               byteCount - n_written );
			
			n_written += sent;
			
			if ( n_written < byteCount  &&  !IsNonblocking() )
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
		catch ( const N::OSStatus& err )
		{
			if ( err == kOTFlowErr )
			{
				if ( IsNonblocking() )
				{
					p7::throw_errno( EAGAIN );
				}
				else
				{
					yield();
					
					const bool may_throw = n_written == 0;
					
					if ( check_signals( may_throw ) )
					{
						// Some bytes were written when a signal was caught
						return n_written;
					}
					
					goto retry;
				}
			}
			
			if ( err == kOTLookErr )
			{
				OTResult look = N::OTLook( itsEndpoint );
				
				switch ( look )
				{
					case T_DISCONNECT:
						ReceiveDisconnect();
						
						p7::throw_errno( ECONNRESET );
					
					default:
						break;
				}
				
				std::fprintf( stderr, "OTResult %d from OTLook()\n", look );
			}
			else
			{
				std::fprintf( stderr, "OSStatus %d from OTSnd()\n", err.Get() );
			}
			
			throw;
		}
		
		return -1;  // Not reached
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
		
		N::OTBind( itsEndpoint, &reqAddr, NULL );
		
		itIsBound    = true;
		itIsListener = true;
	}
	
	std::auto_ptr< IOHandle > OTSocket::Accept( sockaddr& client, socklen_t& len )
	{
		RepairListener();
		
		N::OTSetNonBlocking( itsEndpoint );
		
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( &client );
		call.addr.maxlen = len;
		
		while ( ::OTGetEndpointState( itsEndpoint ) == T_IDLE )
		{
			try_again( IsNonblocking() );
		}
		
		N::OTListen( itsEndpoint, &call );
		
		N::OTSetBlocking( itsEndpoint );
		
		len = call.addr.len;
		
		OTSocket* handle = new OTSocket;
		
		std::auto_ptr< IOHandle > newSocket( handle );
		
		handle->itsPeerAddress.Assign( client, len );
		
		try
		{
			N::OTAccept( itsEndpoint, handle->itsEndpoint, &call );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == kOTLookErr )
			{
				OTResult look = N::OTLook( itsEndpoint );
				
				switch ( look )
				{
					
					default:
						break;
				}
				
				std::fprintf( stderr, "OTResult %d from OTLook() after OTAccept()\n", look );
			}
			else
			{
				std::fprintf( stderr, "OSStatus %d from OTAccept()\n", err.Get() );
			}
			
			throw;
		}
		
		N::OTSetNonBlocking( handle->itsEndpoint );
		
		return newSocket;
	}
	
	void OTSocket::Connect( const sockaddr& server, socklen_t len )
	{
		if ( !itIsBound )
		{
			N::OTBind( itsEndpoint );
			
			itIsBound = true;
		}
		
		TCall sndCall;
		
		::OTMemzero( &sndCall, sizeof (TCall) );
		
		sndCall.addr.buf = reinterpret_cast< unsigned char* >( const_cast< sockaddr* >( &server ) );
		sndCall.addr.len = len;
		
		try
		{
			N::OTConnect( itsEndpoint, sndCall );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == kOTLookErr )
			{
				OTResult look = N::OTLook( itsEndpoint );
				
				switch ( look )
				{
					case T_DISCONNECT:
						N::OTRcvDisconnect( itsEndpoint );
						
						// FIXME:  We should check the discon info, but for now assume
						p7::throw_errno( ECONNREFUSED );
					
					default:
						break;
				}
				
				std::fprintf( stderr, "OTResult %d from OTLook()\n", look );
			}
			else
			{
				std::fprintf( stderr, "OSStatus %d from OTConnect()\n", err.Get() );
			}
			
			throw;
		}
		
		N::OTSetNonBlocking( itsEndpoint );
	}
	
	void OTSocket::ShutdownWriting()
	{
		N::OTSndOrderlyDisconnect( itsEndpoint );
		
		itHasSentFIN = true;
	}
	
	boost::shared_ptr< IOHandle > New_OT_Socket( bool nonblocking )
	{
		return seize_ptr( new OTSocket( nonblocking ) );
	}
	
}

