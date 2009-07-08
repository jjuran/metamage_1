/*	==========
 *	stunnel.cc
 *	==========
 */

// Standard C++
#include <numeric>
#include <string>
#include <vector>

// Standard C/C++
#include <cstdlib>
//#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// OpenSSL
#include <openssl/crypto.h>
//#include <openssl/x509.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/functions/write.hh"
#include "POSeven/types/exit_t.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace O = Orion;
	
	
	struct PeerClosedSocket {};
	struct InvalidEOF       {};
	
	static struct sockaddr_in gRemoteAddress;
	
	int gListenerSocket;
	
	
	static int SSLRead( SSL* session, char* data, std::size_t byteCount )
	{
		int received = SSL_read( session, data, byteCount );
		int err = SSL_get_error( session, received );
		
		switch ( err )
		{
			case SSL_ERROR_NONE:
				return received;
				break;
			
			case SSL_ERROR_ZERO_RETURN:
				throw PeerClosedSocket();
				break;
			
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				throw io::no_input_pending();
				break;
			
			case SSL_ERROR_SYSCALL:
				if ( received == 0 )
				{
					// Invalid EOF
					throw InvalidEOF();
				}
				else if ( received == -1 )
				{
					p7::throw_errno( errno );
				}
				break;
			
			case SSL_ERROR_SSL:
				ERR_print_errors_fp( stderr );
				break;
		}
		
		p7::throw_errno( errno );
		
		// Not reached
		return 0;
	}
	
	static int SSLWrite( SSL* session, const char* data, std::size_t byteCount )
	{
		int sent = SSL_write( session, data, byteCount );
		
		if ( sent < 0 )
		{
			p7::throw_errno( errno );
		}
		
		return sent;
	}
	
	
	static void Startup( const struct sockaddr_in& listener_addr )
	{
		SSLeay_add_ssl_algorithms();
		SSL_load_error_strings();
		
		gListenerSocket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		
		if ( gListenerSocket == -1 )
		{
			std::fprintf( stderr, "Failed to open listener socket: %d\n", errno );
			
			std::exit( 1 );
		}
		
		int bound = bind( gListenerSocket, (const sockaddr*)&listener_addr, sizeof (sockaddr_in) );
		
		int listened = listen( gListenerSocket, 3 );
	}
	
	static void Poll( SSL* client_session, int server_socket )
	{
		bool active = false;
		bool isComplete = false;  // FIXME:  Not used
		
		enum { dataSize = 1024 };
		char data[dataSize];
		int bytes;
		
		sleep( 0 );
		
		try
		{
			while ( true )
			{
				bytes = SSLRead( client_session, data, dataSize );
				write( server_socket, data, bytes);
				active = true;
				if (bytes != dataSize)  break;
			}
		}
		catch ( const io::no_input_pending& )
		{
		}
		catch ( PeerClosedSocket& )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Remote peer closed socket\n" ) );
			isComplete = true;
		}
		catch ( InvalidEOF& )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "EOF received in violation of SSL protocol\n" ) );
			isComplete = true;
		}
		catch ( p7::errno_t& error )
		{
			std::string message = "Remote read error: ";
			
			message += NN::Convert< std::string >( error.Get() );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message.data(), message.size() );
			isComplete = true;
		}
		
		try
		{
			while (  ( bytes = read( server_socket, data, dataSize ) )  >  0  )
			{
				if ( SSLWrite( client_session, data, bytes ) != bytes )
				{
					p7::write( p7::stderr_fileno, STR_LEN( "Remote underwrite\n" ) );
					isComplete = true;
					break;
				}
				
				active = true;
				
				if ( bytes != dataSize )  break;
			}
		}
		catch ( const io::no_input_pending& )
		{
			
		}
		catch ( PeerClosedSocket& )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Local peer closed socket\n" ) );
			isComplete = true;
		}
		catch ( p7::errno_t& error )
		{
			std::string message = "Local read error: ";
			
			message += NN::Convert< std::string >( error.Get() );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message.data(), message.size() );
			
			isComplete = true;
		}
		
		if ( isComplete )
		{
			exit( 1 );
		}
	}
	
	static int setnonblocking( int fd )
	{
		int nonblocking = true;
		
		return ioctl( fd, FIONBIO, &nonblocking );
	}
	
	static void WaitForClients()
	{
		struct sockaddr_in from_addr;
		socklen_t len = sizeof from_addr;
		
		int server_socket = -1;
		errno = 0;
		
		while ( server_socket == -1 && errno == 0  )
		{
			sleep( 1 );
			server_socket = accept( gListenerSocket, (sockaddr*)&from_addr, &len );
		}
		
		if ( server_socket == -1 )
		{
			std::fprintf( stderr, "Failed to accept incoming connection: %d\n", errno );
			
			return;
		}
		
		std::fprintf( stderr, "Connection from %s, port %d.\n",
		                                       "?",     from_addr.sin_port );
		
		setnonblocking( server_socket );
		
		int client_socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		
		int connected = connect( client_socket, (const sockaddr*)&gRemoteAddress, sizeof (sockaddr_in) );
		int unblocked = setnonblocking( client_socket );
		
		SSL_METHOD* mySSLMethod  = SSLv3_client_method();
		SSL_CTX*    mySSLContext = SSL_CTX_new( mySSLMethod  );
		SSL*        mySSLSession = SSL_new    ( mySSLContext );
		
		SSL_set_fd( mySSLSession, client_socket );
		
		int error = SSL_connect( mySSLSession );
		
		while ( true )
		{
			Poll( mySSLSession, server_socket );
		}
	}
	
	
	static struct in_addr ResolveHostname( const char* hostname )
	{
		hostent* hosts = gethostbyname( hostname );
		
		if ( !hosts || h_errno )
		{
			std::string message = "Domain name lookup failed: ";
			
			message += NN::Convert< std::string >( h_errno );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message.data(), message.size() );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return addr;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		const char* remote_host;
		
		short remote_port;
		short listener_port;
		
		O::BindOption( "--lport",  listener_port );
		O::BindOption( "--remote", remote_host   );
		O::BindOption( "--rport",  remote_port   );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
		std::size_t argCount = O::FreeArgumentCount();
		
		if ( listener_port == 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Invalid listener port\n" ) );
			return 1;
		}
		
		if ( remote_port == 0 )
		{
			remote_port = listener_port;
		}
		
		struct in_addr ip = ResolveHostname( remote_host );
		
		struct sockaddr_in remoteAddr;
		
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port   = remote_port;
		remoteAddr.sin_addr   = ip;
		
		gRemoteAddress = remoteAddr;
		
		p7::write( p7::stdout_fileno, STR_LEN( "Secure tunnel daemon starting up..." ) );
		
		unsigned int listen_ip = INADDR_ANY;
		
		struct sockaddr_in listenAddr;
		
		listenAddr.sin_family = AF_INET;
		listenAddr.sin_port = listener_port;
		listenAddr.sin_addr.s_addr = listen_ip;
		
		Startup( listenAddr );
		
		p7::write( p7::stdout_fileno, STR_LEN( " done.\n" ) );
		
		WaitForClients();
		
		return 0;
	}
	
}

