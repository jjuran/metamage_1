/*
	dup.hh
	------
*/


#ifndef POSEVEN_FUNCTIONS_DUP_HH
#define POSEVEN_FUNCTIONS_DUP_HH

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
#endif


namespace poseven
{
	
	nucleus::owned< fd_t > dup( fd_t old_fd );
	
}

#endif

