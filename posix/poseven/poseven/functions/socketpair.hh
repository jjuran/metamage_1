/*
	socketpair.hh
	-------------
*/

#ifndef POSEVEN_FUNCTIONS_SOCKETPAIR_HH
#define POSEVEN_FUNCTIONS_SOCKETPAIR_HH

// poseven
#ifndef POSEVEN_TYPES_FDPAIR_HH
#include "poseven/types/fd_pair.hh"
#endif
#ifndef POSEVEN_TYPES_PROTOCOLFAMILY_HH
#include "poseven/types/protocol_family.hh"
#endif
#ifndef POSEVEN_TYPES_SOCKETTYPE_HH
#include "poseven/types/socket_type.hh"
#endif


namespace poseven
{
	
	fd_pair socketpair( protocol_family  domain,
	                    socket_type      type );
	
}

#endif
