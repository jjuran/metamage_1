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
				catch ( ... )
				{
				}
				
				break;
			
			case kOTProviderWillClose:
				break;
			
			case kOTProviderIsClosed:
				break;
			
			default:
				break;
		}
	}
	
	OTSocket::OTSocket( bool isBlocking ) : itsEndpoint( N::OTOpenEndpoint( N::OTCreateConfiguration( "tcp" ) ) ),
	                                        itIsBound( false )
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
		if ( !IsNonblocking() )
		{
			// OTRcv() will block until ALL bytes requested are received
			// (unlike read()), so we block only while reading the first byte.
			
			std::size_t bytes = 0;
			
			try
			{
				// Always 1 byte (unless there's an error); may block
				bytes = N::OTRcv( itsEndpoint, data, 1 );
			}
			catch ( const N::OSStatus& err )
			{
				if ( err == kOTLookErr )
				{
					OTResult look = N::OTLook( itsEndpoint );
					
					switch ( look )
					{
						case T_ORDREL:
							N::OTRcvOrderlyDisconnect( itsEndpoint );
							
							return 0;
						
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
			
			SetNonblocking();
			
			try
			{
				// Read whatever's left, nonblocking
				bytes += N::OTRcv( itsEndpoint, data + 1, byteCount - 1 );
			}
			catch ( const io::no_input_pending& ) {}  // There was only one byte
			catch ( ... )
			{
				// FIXME:  This smells
				ClearNonblocking();
				throw;
			}
			
			ClearNonblocking();
			
			// And return one or more bytes.  Yay, POSIX semantics!
			return bytes;
		}
		
		return N::OTRcv( itsEndpoint, data, byteCount );
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
				
				switch ( look )
				{
					case T_ORDREL:
						N::OTRcvOrderlyDisconnect( itsEndpoint );
						
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

