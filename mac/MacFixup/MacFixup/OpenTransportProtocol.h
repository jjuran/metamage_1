/*
	MacFixup/OpenTransportProtocol.h
	--------------------------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef MACFIXUP_OPENTRANSPORTPROTOCOL_H
#define MACFIXUP_OPENTRANSPORTPROTOCOL_H

#ifdef __OPENTRANSPORTPROTOCOL__
#error OpenTransportProtocol.h included too early to divert duplicate definitions
#endif

#define uid_t    _OpenTransportProtocol_uid_t
#define gid_t    _OpenTransportProtocol_gid_t
#define dev_t    _OpenTransportProtocol_dev_t
#define caddr_t  _OpenTransportProtocol_caddr_t

#include <CIncludes/OpenTransportProtocol.h>

#undef uid_t
#undef gid_t
#undef dev_t
#undef caddr_t

#endif

