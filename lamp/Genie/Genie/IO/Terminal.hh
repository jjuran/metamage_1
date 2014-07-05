/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	const pid_t no_pgid = 0x7fffffff;
	
}

namespace relix
{
	
	struct terminal_extra
	{
		vfs::filehandle*  tty;
		pid_t             pgid;
		bool              disconnected;
	};
	
}

namespace Genie
{
	
	class TerminalHandle : public vfs::filehandle
	{
		public:
			TerminalHandle( const vfs::node& tty_file );
	};
	
}

namespace relix
{
	
	vfs::filehandle_ptr new_terminal( const vfs::node& tty_file );
	
}

#endif

