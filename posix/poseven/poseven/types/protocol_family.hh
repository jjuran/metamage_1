/*
	protocol_family.hh
	------------------
*/

#ifndef POSEVEN_TYPES_PROTOCOLFAMILY_HH
#define POSEVEN_TYPES_PROTOCOLFAMILY_HH

// POSIX
#include <sys/socket.h>

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace poseven
{
	
	enum protocol_family
	{
		pf_unix  = PF_UNIX,
		pf_local = PF_LOCAL,
		pf_inet  = PF_INET,
		pf_inet6 = PF_INET6,
		
	#ifdef PF_IPX
		
		pf_ipx   = PF_IPX,
		
	#endif
		
	#ifdef PF_APPLETALK
		
		pf_appletalk = PF_APPLETALK,
		
	#endif
		
		protocol_family_max = nucleus::enumeration_traits< int >::max
	};
	
}

#endif
