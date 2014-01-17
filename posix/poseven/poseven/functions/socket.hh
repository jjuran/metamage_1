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

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
#endif
#ifndef POSEVEN_TYPES_PROTOCOLFAMILY_HH
#include "poseven/types/protocol_family.hh"
#endif
#ifndef POSEVEN_TYPES_SOCKETTYPE_HH
#include "poseven/types/socket_type.hh"
#endif


namespace poseven
{
	
	nucleus::owned< fd_t > socket( protocol_family  domain,
	                               socket_type      type );
	
}

#endif

