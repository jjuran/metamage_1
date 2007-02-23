/*	===============
 *	SocketStream.cc
 *	===============
 */

#include "Genie/IO/SocketStream.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// Genie
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	
	static pascal void YieldingNotifier( void* contextPtr,
	                                     OTEventCode code,
	                                     OTResult result,
	                                     void* cookie );
	
	static pascal void YieldingNotifier( void*,
	                                     OTEventCode code,
	                                     OTResult,
	                                     void* )
	{
		switch ( code )
		{
			case kOTSyncIdleEvent:
				try
				{
					Yield();
				}
				catch ( ... ) {}
				break;
			
			case kOTProviderWillClose:
				break;
			
			case kOTProviderIsClosed:
				break;
			
			default:
				break;
		}
	}
	
	SocketHandle::SocketHandle( bool isBlocking )
	: endpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
	  isBound( false )
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( YieldingNotifier );
		
		// The new endpoint is synchronous and (by default) nonblocking.
		// New Berkeley sockets are blocking.
		
		if ( isBlocking )
		{
			N::OTSetBlocking( endpoint );
		}
		
		N::OTInstallNotifier( endpoint, gNotifyUPP, NULL );
		
		N::OTUseSyncIdleEvents( endpoint, true );
	}
	
	SocketHandle::~SocketHandle()
	{
		if ( isBound )
		{
			(void)::OTUnbind( endpoint );
		}
	}
	
	unsigned int SocketHandle::SysPoll() const
	{
		::OTResult state = ::OTGetEndpointState( endpoint );
		
		bool canRead = true;
		
		if ( state == T_IDLE )
		{
			canRead = false;
		}
		else if ( state == T_DATAXFER )
		{
			::OTByteCount count;
			
			canRead = ::OTCountDataBytes( endpoint, &count ) == noErr;
		}
		
		return (canRead ? kPollRead : 0) | kPollWrite;
	}
	
	int SocketHandle::SysRead( char* data, std::size_t byteCount )
	{
		if ( IsBlocking() )
		{
			// OTRcv() will block until ALL bytes requested are received
			// (unlike read()), so we block only while reading the first byte.
			
			std::size_t bytes = 0;
			
			try
			{
				// Always 1 byte (unless there's an error); may block
				bytes = N::OTRcv( endpoint, data, 1 );
			}
			catch ( const N::OSStatus& err )
			{
				if ( err == kOTLookErr )
				{
					OTResult look = N::OTLook( endpoint );
					
					switch ( look )
					{
						case T_ORDREL:
							N::OTRcvOrderlyDisconnect( endpoint );
							
							throw io::end_of_input();
						
						default:
							break;
					}
					
					std::fprintf( stderr, "OTResult %d from OTLook()\n", look );
				}
				else
				{
					std::fprintf( stderr, "OSStatus %d from OTRcv()\n", err.Get() );
				}
				
				throw;
			}
			
			SetNonBlocking();
			
			try
			{
				// Read whatever's left, nonblocking
				bytes += N::OTRcv( endpoint, data + 1, byteCount - 1 );
			}
			catch ( const io::no_input_pending& ) {}  // There was only one byte
			catch ( ... )
			{
				// FIXME:  This smells
				SetBlocking();
				throw;
			}
			
			SetBlocking();
			
			// And return one or more bytes.  Yay, POSIX semantics!
			return bytes;
		}
		
		return N::OTRcv( endpoint, data, byteCount );
	}
	
	int SocketHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return N::OTSnd( endpoint, data, byteCount );
	}
	
	void SocketHandle::Bind( ConstSockAddrParam local, socklen_t len )
	{
		if ( len != sizeof (InetAddress) )
		{
			// Fail
		}
		
		socketAddress.Assign( local, len );
	}
	
	void SocketHandle::Listen( int backlog )
	{
		if ( socketAddress.Get() == NULL )
		{
			// Fail
		}
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = reinterpret_cast< unsigned char* >( socketAddress.Get() );
		reqAddr.addr.len = socketAddress.Len();
		reqAddr.qlen = backlog;
		
		N::OTBind( endpoint, &reqAddr, NULL );
		
		isBound = true;
	}
	
	std::auto_ptr< SocketHandle > SocketHandle::Accept( SockAddrParam client, socklen_t& len )
	{
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( client );
		call.addr.maxlen = len;
		
		N::OTListen( endpoint, &call );
		
		len = call.addr.len;
		
		SocketHandle* handle = new SocketHandle;
		
		std::auto_ptr< SocketHandle > newSocket( handle );
		
		newSocket->peerAddress.Assign( client, len );
		
		N::OTAccept( endpoint, newSocket->endpoint, &call );
		
		return newSocket;
	}
	
	Accept_Result SocketHandle::Accept()
	{
		InetAddress address;
		socklen_t len = sizeof address;
		
		std::auto_ptr< SocketHandle > newSocket = Accept( &address, len );
		
		return Accept_Result( boost::shared_ptr< SocketHandle >( newSocket ),
		                      SocketAddress( &address, len ) );
	}
	
	void SocketHandle::Connect( ConstSockAddrParam server, socklen_t len )
	{
		if ( !isBound )
		{
			N::OTBind( endpoint );
			isBound = true;
		}
		
		TCall sndCall;
		
		::OTMemzero( &sndCall, sizeof (TCall) );
		
		sndCall.addr.buf = reinterpret_cast< unsigned char* >( const_cast< InetAddress* >( server ) );
		sndCall.addr.len = len;
		
		N::OTConnect( endpoint, sndCall );
	}
	
}

