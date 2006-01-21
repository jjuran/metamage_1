/*	===============
 *	SocketHandle.cc
 *	===============
 */

#include "Genie/SocketHandle.hh"

// Nitrogen core
#include "Nitrogen/OnlyOnce.h"

// POSeven
#include "POSeven/Errno.h"

// Io
#include "Io/Exceptions.hh"

// Genie
#include "Genie/ResourceTable.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	typedef ResourceTable< SocketHandle > SocketTable;
	
	void RegisterSocketRefMod();
	void RegisterSocketRefMod()
	{
		RegisterIOType( kSocketDescriptor,
		                SocketTable::RefMod,
		                SocketTable::Read,
		                SocketTable::Write,
		                SocketTable::Poll );
	}
	
	
	IORef NewSocket( bool blockingMode )
	{
		N::OnlyOnce< RegisterSocketRefMod >();
		
		SocketHandle* socket = new SocketHandle( blockingMode );
		
		std::size_t offset = SocketTable::Add( std::auto_ptr< SocketHandle >( socket ) );
		
		return IORef( kSocketDescriptor, offset, socket );
	}
	
	
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
				catch ( P7::Errno& err )
				{
					// FIXME
				}
				catch ( ... ) {}
				break;
			
			default:
				break;
		}
	}
	
	SocketHandle::SocketHandle( bool blocking )
	:
		endpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
		fBound( false )
	{
		static OTNotifyUPP gNotifyUPP = ::NewOTNotifyUPP( YieldingNotifier );
		
		// The new endpoint is synchronous and (by default) nonblocking.
		// New Berkeley sockets are blocking.
		
		if ( blocking )
		{
			N::OTSetBlocking( endpoint );
		}
		
		N::OTInstallNotifier( endpoint, gNotifyUPP, NULL );
		
		N::OTUseSyncIdleEvents( endpoint, true );
	}
	
	SocketHandle::~SocketHandle()
	{
		if ( fBound )
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
			std::size_t count;
			
			canRead = ::OTCountDataBytes( endpoint, &count ) == noErr;
		}
		
		return (canRead ? kPollRead : 0) | kPollWrite;
	}
	
	int SocketHandle::SysRead( char* data, std::size_t byteCount )
	{
		try
		{
			bool blocking = N::OTIsBlocking( endpoint );
			
			if ( blocking )
			{
				std::size_t bytes = N::OTRcv( endpoint, data, 1 );  // Always 1
				
				N::OTSetNonBlocking( endpoint );
				
				try
				{
					bytes += N::OTRcv( endpoint, data + 1, byteCount - 1 );
				}
				catch ( N::OTNoDataErr& ) {}  // There was only one byte
				
				N::OTSetBlocking( endpoint );
				
				return bytes;
			}
			
			return N::OTRcv( endpoint, data, byteCount );
		}
		catch ( N::OTNoDataErr )
		{
			throw Io::NoDataPending();
		}
		
		// Not reached
		return -1;
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
		
		fSocketAddress.Assign( local, len );
	}
	
	void SocketHandle::Listen( int backlog )
	{
		if ( fSocketAddress.Get() == NULL )
		{
			// Fail
		}
		
		TBind reqAddr;
		
		::OTMemzero( &reqAddr, sizeof (TBind) );
		
		reqAddr.addr.buf = reinterpret_cast< unsigned char* >( fSocketAddress.Get() );
		reqAddr.addr.len = fSocketAddress.Len();
		reqAddr.qlen = backlog;
		
		N::OTBind( endpoint, &reqAddr, NULL );
		
		fBound = true;
	}
	
	IORef SocketHandle::Accept( SockAddrParam client, socklen_t& len )
	{
		TCall call;
		
		::OTMemzero( &call, sizeof (TCall) );
		
		call.addr.buf = reinterpret_cast< unsigned char* >( client );
		call.addr.maxlen = len;
		
		try
		{
			N::OTListen( endpoint, &call );
		}
		catch ( N::OTNoDataErr )
		{
			throw Io::NoDataPending();
		}
		
		len = call.addr.len;
		
		//N::Owned< SocketHandle* > newSocket = N::Owned< SocketHandle* >::Seize( new SocketHandle );
		
		SocketHandle* handle = new SocketHandle;
		
		std::auto_ptr< SocketHandle > newSocket( handle );
		
		newSocket->fPeerAddress.Assign( client, len );
		
		N::OTAccept( endpoint, newSocket->endpoint, &call );
		
		std::size_t offset = SocketTable::Add( newSocket );
		
		return IORef( kSocketDescriptor, offset, handle );
	}
	
	Accept_Result SocketHandle::Accept()
	{
		InetAddress address;
		socklen_t len = sizeof address;
		
		IORef newSocket = Accept( &address, len );
		
		return Accept_Result( newSocket, SocketAddress( &address, len ) );
	}
	
	void SocketHandle::Connect( ConstSockAddrParam server, socklen_t len )
	{
		if ( !fBound )
		{
			N::OTBind( endpoint );
			fBound = true;
		}
		
		TCall sndCall;
		
		::OTMemzero( &sndCall, sizeof (TCall) );
		
		sndCall.addr.buf = reinterpret_cast< unsigned char* >( const_cast< InetAddress* >( server ) );
		sndCall.addr.len = len;
		
		N::OTConnect( endpoint, sndCall );
	}
	
}

