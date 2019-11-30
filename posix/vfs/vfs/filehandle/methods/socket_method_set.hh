/*
	socket_method_set.hh
	--------------------
*/

#ifndef VFS_METHODS_SOCKETMETHODSET_HH
#define VFS_METHODS_SOCKETMETHODSET_HH

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_ptr.hh"

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace vfs
{
	
	class filehandle;
	
	
	typedef void (*bind_method)( filehandle*, const sockaddr* addr, socklen_t len );
	
	typedef void (*listen_method)( filehandle*, int backlog );
	
	typedef filehandle_ptr (*accept_method)( filehandle*, sockaddr* addr, socklen_t* len );
	
	typedef void (*connect_method)( filehandle*, const sockaddr* addr, socklen_t len );
	
	typedef void (*shutdown_method)( filehandle*, int how );
	
	typedef const sockaddr* (*getpeername_method)( filehandle* );
	typedef const sockaddr* (*getsockname_method)( filehandle* );
	
	typedef int (*conveying_method)( filehandle* );
	
	struct socket_method_set
	{
		bind_method      bind;
		listen_method    listen;
		accept_method    accept;
		connect_method   connect;
		shutdown_method  shutdown;
		
		getsockname_method  getsockname;
		getpeername_method  getpeername;
		
		conveying_method  conveying;
	};
	
}

#endif
