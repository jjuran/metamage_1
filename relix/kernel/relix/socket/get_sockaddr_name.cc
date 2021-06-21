/*
	get_sockaddr_name.cc
	--------------------
*/

#include "relix/socket/get_sockaddr_name.hh"

// more-libc
#include "more/string.h"


namespace relix
{
	
	static inline socklen_t sizeof_sockaddr( const sockaddr& addr )
	{
		return sizeof (sockaddr);
	}
	
	void get_sockaddr_name( const sockaddr& addr, struct sockaddr* name, socklen_t* namelen )
	{
		const socklen_t size = sizeof_sockaddr( addr );
		
		const size_t n = *namelen < size ? *namelen : size;
		
		mempcpy( name, &addr, n );
		
		*namelen = size;
	}
	
}
