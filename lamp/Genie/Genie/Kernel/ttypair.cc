/*
	ttypair.cc
	----------
*/

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/config/pts.hh"
#include "relix/fs/pseudotty.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"


/*
	ttypair() is analogous to socketpair() and creates a pseudo-tty device.
	File descriptors refering to the master and slave respectively are returned
	in fds.
*/

namespace relix
{
	
	static int ttypair( int fds[ 2 ] )
	{
		try
		{
			vfs::filehandle_ptr master, slave;
			
			new_pseudotty_pair( master, slave );
			
			int master_fd = first_free_fd( 3 );
			int slave_fd  = first_free_fd( master_fd + 1 );
			
			assign_fd( master_fd, *master );
			assign_fd( slave_fd,  *slave  );
			
			fds[ 0 ] = master_fd;
			fds[ 1 ] = slave_fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

using namespace relix;

#pragma force_active on

#if CONFIG_PTS

REGISTER_SYSTEM_CALL( ttypair );

#endif

#pragma force_active reset
