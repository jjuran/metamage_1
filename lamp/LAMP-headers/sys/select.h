/*	============
 *	sys/select.h
 *	============
 */

#ifndef KEROSENE_SYS_SELECT_H
#define KEROSENE_SYS_SELECT_H

#include "sys/time.h"

#if 0
#include "bool.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	#define FD_SETSIZE 256
	
	struct fd_set
	{
		unsigned long data[ FD_SETSIZE / 32 ];
	};
	
	typedef struct fd_set fd_set;
	
	inline bool FD_ISSET( int fd, const fd_set* set )
	{
		return set->data[ fd / 32 ] & 1 << fd % 32;
	}
	
	inline void FD_CLR( int fd, fd_set* set )
	{
		set->data[ fd / 32 ] &= ~(1 << fd % 32);
	}
	
	inline void FD_SET( int fd, fd_set* set )
	{
		set->data[ fd / 32 ] |= 1 << fd % 32;
	}
	
	/*
	inline void FD_ZERO( fd_set* set )
	{
		
	}
	*/
	
	#define FD_ZERO(set)  memset( set, 0, sizeof (fd_set) )
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout );
	
#ifdef __cplusplus
}
#endif

#endif

#endif

