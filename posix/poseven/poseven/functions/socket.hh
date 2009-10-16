// socket.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SOCKET_HH
#define POSEVEN_FUNCTIONS_SOCKET_HH

// POSIX
#include <sys/socket.h>

// Nucleus
#include "nucleus/flag_ops.hh"

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	enum protocol_family
	{
		pf_unix  = PF_UNIX,
		pf_local = PF_LOCAL,
		pf_inet  = PF_INET,
		pf_inet6 = PF_INET6,
		pf_ipx   = PF_IPX,
		
		
	#ifdef PF_APPLETALK
		
		pf_appletalk = PF_APPLETALK,
		
	#endif
		
		protocol_family_max = nucleus::enumeration_traits< int >::max
	};
	
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
	
	inline nucleus::owned< fd_t > socket( protocol_family  domain,
	                                      socket_type      type )
	{
		const int fd = throw_posix_result( ::socket( domain, type, 0 ) );
		
		return nucleus::owned< fd_t >::seize( fd_t( fd ) );
	}
	
}

#endif

