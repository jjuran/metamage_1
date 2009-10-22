/*
	write_all.hh
	------------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_WRITEALL_HH
#define POSEVEN_EXTRAS_WRITEALL_HH

// POSIX
#include <sys/types.h>

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	ssize_t write_some( fd_t fd, const char* buffer, size_t n_bytes );
	
	ssize_t write_all( fd_t fd, const char* buffer, size_t n_bytes );
	
}

#endif

