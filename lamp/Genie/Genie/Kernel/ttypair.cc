/*
	ttypair.cc
	----------
*/

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/config/pts.hh"
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
		
		AssignFileDescriptor( master_fd, master );
		AssignFileDescriptor( slave_fd,  slave  );
		
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

