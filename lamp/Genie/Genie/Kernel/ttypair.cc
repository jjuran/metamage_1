/*
	ttypair.cc
	----------
*/

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/config/pts.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/IO/PseudoTTY.hh"


/*
	ttypair() is analogous to socketpair() and creates a pseudo-tty device.
	File descriptors refering to the master and slave respectively are returned
	in fds.
*/

static int ttypair( int fds[ 2 ] )
{
	using namespace Genie;
	
	try
	{
		IOPtr master, slave;
		
		GetNewPseudoTTYPair( master, slave );
		
		int master_fd = relix::first_free_fd( 3 );
		int slave_fd  = relix::first_free_fd( master_fd + 1 );
		
		relix::assign_fd( master_fd, *master );
		relix::assign_fd( slave_fd,  *slave  );
		
		fds[ 0 ] = master_fd;
		fds[ 1 ] = slave_fd;
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}

#if CONFIG_PTS

REGISTER_SYSTEM_CALL( ttypair );

#endif

