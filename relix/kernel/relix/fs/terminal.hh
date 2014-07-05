/*
	terminal.hh
	-----------
*/

#ifndef RELIX_FS_TERMINAL_HH
#define RELIX_FS_TERMINAL_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace relix
{
	
	struct terminal_extra
	{
		vfs::filehandle*  tty;
		pid_t             pgid;
		bool              disconnected;
	};
	
	vfs::filehandle_ptr new_terminal( const vfs::node& tty_file );
	
}

#endif
