/*
	socket_type.hh
	--------------
*/

#ifndef POSEVEN_TYPES_SOCKETTYPE_HH
#define POSEVEN_TYPES_SOCKETTYPE_HH

// POSIX
#include <sys/socket.h>

// nucleus
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace poseven
{
	
	enum socket_type
	{
		sock_stream = SOCK_STREAM,
		sock_dgram  = SOCK_DGRAM,
		
	#ifdef SOCK_NONBLOCK
		
		sock_nonblock = SOCK_NONBLOCK,
		
	#endif
		
	#ifdef SOCK_CLOEXEC
		
		sock_cloexec = SOCK_CLOEXEC,
		
	#endif
		
		socket_type_max = nucleus::enumeration_traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( socket_type )
	
}

#endif
