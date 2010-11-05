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
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
#endif


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
	
	nucleus::owned< fd_t > socket( protocol_family  domain,
	                               socket_type      type );
	
}

#endif

