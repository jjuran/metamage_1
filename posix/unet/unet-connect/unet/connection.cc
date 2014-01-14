/*
	connection.cc
	-------------
*/

#include "unet/connection.hh"

// POSIX
#include <unistd.h>


namespace unet
{
	
	connection::~connection()
	{
		::close( its_input  );
		::close( its_output );
	}
	
}
