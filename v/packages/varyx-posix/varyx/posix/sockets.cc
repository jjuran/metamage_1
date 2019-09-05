/*
	sockets.cc
	----------
*/

#include "varyx/posix/sockets.hh"

// POSIX
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

// vlib
#include "vlib/types.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/exception.hh"
#include "varyx/posix/file_descriptor.hh"

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	class addrinfo_cleanup
	{
		private:
			addrinfo* its_ai;
			
			// non-copyable
			addrinfo_cleanup           ( const addrinfo_cleanup& );
			addrinfo_cleanup& operator=( const addrinfo_cleanup& );
		
		public:
			addrinfo_cleanup( addrinfo* ai ) : its_ai( ai )
			{
			}
			
			~addrinfo_cleanup()
			{
				::freeaddrinfo( its_ai );
			}
	};
	
	static
	void gai_error( const char* node, const char* service, int err )
	{
		Value error = mapping( "error",    Integer(               err   ) );
		Value desc  = mapping( "desc",     String ( gai_strerror( err ) ) );
		Value node_ = mapping( "node",     String (              node   ) );
		Value svc   = mapping( "service",  String (           service   ) );
		
		Value exception( error, Value( desc, Value( node_, svc ) ) );
		
		throw_exception_object( exception );
	}
	
	
	static
	Value v_tcpconnect( const Value& v )
	{
		list_iterator args( v );
		
		const char* node    = args.use().string().c_str();
		const char* service = args.get().string().c_str();
		
		addrinfo hints = { 0 };
		
		hints.ai_socktype = SOCK_STREAM;
		
		addrinfo* ai;
		
		int err = getaddrinfo( node, service, &hints, &ai );
		
		if ( err )
		{
			gai_error( node, service, err );
		}
		
		addrinfo_cleanup cleanup( ai );
		
		int fd = socket( ai->ai_family, SOCK_STREAM, 0 );
		
		if ( fd < 0 )
		{
			fd_error( fd );
		}
		
		int nok = connect( fd, ai->ai_addr, ai->ai_addrlen );
		
		if ( nok < 0 )
		{
			close( fd );
			
			fd_error( fd );
		}
		
		return FileDescriptor( fd, automatic );
	}
	
	static
	Value v_tcplisten( const Value& v )
	{
		const char* service = v.string().c_str();
		
		addrinfo hints = { 0 };
		
		hints.ai_flags    = AI_PASSIVE;
		hints.ai_socktype = SOCK_STREAM;
		
		addrinfo* ai;
		
		int err = getaddrinfo( NULL, service, &hints, &ai );
		
		if ( err )
		{
			gai_error( "<null>", service, err );
		}
		
		addrinfo_cleanup cleanup( ai );
		
		int fd = socket( ai->ai_family, SOCK_STREAM, 0 );
		
		if ( fd < 0 )
		{
			fd_error( fd );
		}
		
		FileDescriptor result( fd, automatic );
		
		int on = 1;
		setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on );
		
		int nok = bind( fd, ai->ai_addr, ai->ai_addrlen );
		
		if ( nok < 0 )
		{
			fd_error( fd );
		}
		
		const int backlog = 5;
		
		nok = listen( fd, backlog );
		
		if ( nok < 0 )
		{
			fd_error( fd );
		}
		
		return result;
	}
	
	static
	Value v_accept( const Value& v )
	{
		const FileDescriptor& filedesc = (const FileDescriptor&) v;
		
		int listener_fd = filedesc.get();
		
		sockaddr_storage addr;
		socklen_t len = sizeof addr;
		
		int client_fd = accept( listener_fd, (sockaddr*) &addr, &len );
		
		if ( client_fd < 0 )
		{
			fd_error( listener_fd );
		}
		
		const Value fd = FileDescriptor( client_fd, automatic );
		
		const Value address = Packed( plus::string( (char*) &addr, len ) );
		
		return Record( Value( mapping( "socket",  fd ),
		                      mapping( "address", address ) ) );
	}
	
	static const Type fd    = fd_vtype;
	static const Type c_str = c_str_vtype;
	
	static const Value c_str_x2( c_str, c_str );
	
	const proc_info proc_accept     = { "accept",      &v_accept,     &fd };
	const proc_info proc_tcpconnect = { "tcp-connect", &v_tcpconnect, &c_str_x2 };
	const proc_info proc_tcplisten  = { "tcp-listen",  &v_tcplisten,  &c_str };
	
}
}
