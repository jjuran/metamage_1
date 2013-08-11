/*
	ttypair.cc
	----------
*/

// relix-kernel
#include "relix/config/pts.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
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
		
		int master_fd = LowestUnusedFileDescriptor( 3 );
		int slave_fd  = LowestUnusedFileDescriptor( master_fd + 1 );
		
		assign_file_descriptor( master_fd, *master );
		assign_file_descriptor( slave_fd,  *slave  );
		
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

