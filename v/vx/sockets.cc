/*
	sockets.cc
	----------
*/

#include "sockets.hh"

// POSIX
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

// vlib
#include "vlib/types.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// vx
#include "exception.hh"
#include "file_descriptor.hh"


namespace vlib
{
	
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
		
		return FileDescriptor( fd );
	}
	
	static const Type c_str = c_str_vtype;
	
	static const Value c_str_x2( c_str, c_str );
	
	const proc_info proc_tcpconnect = { "tcp-connect", &v_tcpconnect, &c_str_x2 };
	
}
