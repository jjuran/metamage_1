/*
	sockets.hh
	----------
*/

#ifndef VARYX_POSIX_SOCKETS_HH
#define VARYX_POSIX_SOCKETS_HH

// vlib
#include "vlib/proc_info.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::proc_info;
	
	extern const proc_info proc_accept;
	extern const proc_info proc_tcpconnect;
	extern const proc_info proc_tcplisten;
	
}
}

#endif
