/*	===========
 *	OTSocket.cc
 *	===========
 */

#include "Genie/IO/OTSocket.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static pascal void YieldingNotifier( void*        contextPtr,
	                                     OTEventCode  code,
	                                     OTResult     result,
	                                     void*        cookie )
	{
		OTSocket* socket = (OTSocket*) contextPtr;
		
		try
		{
			switch ( code )
			{
				case T_ORDREL:
					socket->ReceiveOrderlyDisconnect();
					
					break;
				
				case kOTSyncIdleEvent:
					Yield( kInterruptNever );  // FIXME
					
					break;
				
				case kOTProviderWillClose:
					break;
				
				case kOTProviderIsClosed:
					break;
				
				default:
					break;
			}
		}
		catch ( ... )
		{
		}
	}
	
	OTSocket::OTSocket( bool isBlocking ) : itsEndpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
	                                        itIsBound       ( false ),
	                                        itHasSentFIN    ( false ),
	                                        itHasReceivedFIN( false )
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( YieldingNotifier );
		
		// The new endpoint is synchronous and (by default) nonblocking.
		// New Berkeley sockets are blocking.
		
		if ( isBlocking )
		{
			N::OTSetBlocking( itsEndpoint );
		}
		
		N::OTInstallNotifier( itsEndpoint, gNotifyUPP, NULL );
		
		N::OTUseSyncIdleEvents( itsEndpoint, true );
	}
	
	OTSocket::~OTSocket()
	{
		if ( itIsBound )
		{
			(void) ::OTUnbind( itsEndpoint );
		}
	}
	
	void OTSocket::ReceiveOrderlyDisconnect()
	{
		N::OTRcvOrderlyDisconnect( itsEndpoint );
		
		itHasReceivedFIN = true;
	}
	
	unsigned int OTSocket::SysPoll() const
	{
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
	
	int OTSocket::SysRead( char* data, std::size_t byteCount )
	{
		if ( itHasReceivedFIN )
		{
			return 0;
		}
		
		const bool socket_is_nonblocking = IsNonblocking();
		
		::OTByteCount n_readable_bytes;
		
		OSStatus err_count;
		
		while ( true )
		{
			err_count = ::OTCountDataBytes( itsEndpoint, &n_readable_bytes );
			
			if ( err_count != kOTNoDataErr )
			{
				break;
			}
			
			if ( socket_is_nonblocking )
			{
				break;
			}
			
			Yield( kInterruptAlways );
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
	
	int OTSocket::SysWrite( const char* data, std::size_t byteCount )
	{
		retry:
		
		try
		{
			return N::OTSnd( itsEndpoint, data, byteCount );
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
			// Fail
		}
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = reinterpret_cast< unsigned char* >( itsSocketAddress.Get() );
		reqAddr.addr.len = itsSocketAddress.Len();
		reqAddr.qlen = backlog;
		
		N::OTBind( itsEndpoint, &reqAddr, NULL );
		
		itIsBound = true;
	}
	
	std::auto_ptr< IOHandle > OTSocket::Accept( sockaddr& client, socklen_t& len )
	{
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( &client );
		call.addr.maxlen = len;
		
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
	}
	
	void OTSocket::ShutdownWriting()
	{
		N::OTSndOrderlyDisconnect( itsEndpoint );
	}
	
}

