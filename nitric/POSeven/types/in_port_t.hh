// in_port_t.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_IN_PORT_T_HH
#define POSEVEN_TYPES_IN_PORT_T_HH

// POSIX
#include <netinet/in.h>

// Nucleus
#include "Nucleus/Enumeration.h"


namespace poseven
{
	
	enum in_port_t
	{
		/*
		ipport_echo       = IPPORT_ECHO,
		ipport_discard    = IPPORT_DISCARD,
		ipport_systat     = IPPORT_SYSTAT,
		ipport_daytime    = IPPORT_DAYTIME,
		ipport_netstat    = IPPORT_NETSTAT,
		ipport_ftp        = IPPORT_FTP,
		ipport_telnet     = IPPORT_TELNET,
		ipport_smtp       = IPPORT_SMTP,
		ipport_timeserver = IPPORT_TIMESERVER,
		ipport_nameserver = IPPORT_NAMESERVER,
		ipport_whois      = IPPORT_WHOIS,
		ipport_mtp        = IPPORT_MTP,
		*/
		
		// ...
		
		in_port_t_max = Nucleus::Enumeration_Traits< ::in_port_t >::max
	};
	
}

#endif

