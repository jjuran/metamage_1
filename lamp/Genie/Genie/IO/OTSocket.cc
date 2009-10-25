/*	===========
 *	OTSocket.cc
 *	===========
 */

#include "Genie/IO/OTSocket.hh"

// Io
#include "io/io.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.h"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static pascal void YieldingNotifier( void*        contextPtr,
	                                     OTEventCode  code,
	                                     OTResult     result,
	                                     void*        cookie )
	{
		try
		{
			switch ( code )
			{
				case kOTSyncIdleEvent:
					// Hack to make sure we don't get starved for events
					Ped::AdjustSleepForTimer( 4 );
					
					Yield( kInterruptNever );  // FIXME
					
					break;
				
				case kOTProviderWillClose:
					break;
				
				case kOTProviderIsClosed:
					break;
				
				default:
					break;
			}
			
			if ( OTSocket* socket = (OTSocket*) contextPtr )
			{
				switch ( code )
				{
					case T_ORDREL:
						socket->ReceiveOrderlyDisconnect();
						
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
	
	static void SetUpEndpoint( N::EndpointRef endpoint )
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( YieldingNotifier );
		
		// The new endpoint is synchronous and (by default) nonblocking.
		
		// The underlying endpoint is always nonblocking for send and recv
		// and blocking for connect and listen (until we add support)
		
		N::OTSetBlocking( endpoint );
		
		N::OTInstallNotifier( endpoint, gNotifyUPP, NULL );
		
		N::OTUseSyncIdleEvents( endpoint, true );
	}
	
	OTSocket::OTSocket( bool nonblocking ) : SocketHandle( nonblocking ),
	                                         itsEndpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
	                                         itsBacklog(),
	                                         itIsBound       ( false ),
	                                         itIsListener    ( false ),
	                                         itHasSentFIN    ( false ),
	                                         itHasReceivedFIN( false ),
	                                         itHasReceivedRST( false )
	{
		SetUpEndpoint( itsEndpoint );
	}
	
	OTSocket::~OTSocket()
	{
		if ( itIsBound )
		{
			(void) ::OTUnbind( itsEndpoint );
		}
	}
	
	void OTSocket::ReceiveDisconnect()
	{
		N::OTRcvDisconnect( itsEndpoint );
		
		itHasReceivedRST = true;
	}
	
	void OTSocket::ReceiveOrderlyDisconnect()
	{
		N::OTRcvOrderlyDisconnect( itsEndpoint );
		
		itHasReceivedFIN = true;
	}
	
	bool OTSocket::RepairListener()
	{
		if ( itIsListener && ::OTGetEndpointState( itsEndpoint ) == 0 )
		{
			Yield( kInterruptAlways );
			
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
			err_count = ::OTCountDataBytes( itsEndpoint, &n_readable_bytes );
			
			if ( err_count != kOTNoDataErr )
			{
				break;
			}
			
			if ( IsNonblocking() )
			{
				break;
			}
			
			// Hack to make sure we don't get starved for events
			Ped::AdjustSleepForTimer( 4 );
			
			Yield( kInterruptAlways );
		}
		
		if ( err_count == kOTLookErr )
		{
			OTResult look = N::OTLook( itsEndpoint );
			
			if ( look == T_ORDREL )
			{
				ReceiveOrderlyDisconnect();
				
				return 0;
			}
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
		retry:
		
		try
		{
			ssize_t sent = N::OTSnd( itsEndpoint, data, byteCount );
			
			if ( sent < byteCount  &&  !IsNonblocking() )
			{
				data += sent;
				
				byteCount -= sent;
				
				Yield( kInterruptNever );
				
				goto retry;
			}
			
			return sent;
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
					Yield( kInterruptNever );
					
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
						
						{
							Process& current = CurrentProcess();
							
							current.Raise( SIGPIPE );
							
							current.HandlePendingSignals( kInterruptNever );
						}
						
						p7::throw_errno( EPIPE );
						
					case T_ORDREL:
						ReceiveOrderlyDisconnect();
						
						goto retry;
					
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
			throw N::OTOutStateErr();
		}
		
		itsBacklog = backlog;
		
		// Throw out our tcp-only endpoint and make one with tilisten prepended
		itsEndpoint = N::OTOpenEndpoint( N::OTCreateConfiguration( "tilisten,tcp" ) );
		
		SetUpEndpoint( itsEndpoint );
		
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
		
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( &client );
		call.addr.maxlen = len;
		
		N::OTListen( itsEndpoint, &call );
		
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
	
}

