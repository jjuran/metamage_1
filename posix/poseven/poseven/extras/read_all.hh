/*
	read_all.hh
	-----------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_READALL_HH
#define POSEVEN_EXTRAS_READALL_HH

// POSIX
#include <sys/types.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	ssize_t read_some( fd_t fd, char* buffer, size_t n_bytes );
	
	ssize_t read_all( fd_t fd, char* buffer, size_t n_bytes );
	
}

#endif

