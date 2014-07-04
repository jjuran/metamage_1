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
	
	
	class TerminalHandle : public vfs::filehandle
	{
		private:
			vfs::filehandle_ptr  its_tty;
			pid_t                its_process_group_id;
			bool                 it_is_disconnected;
		
		public:
			TerminalHandle( const vfs::node& tty_file );
			
			~TerminalHandle();
			
			bool disconnected() const  { return it_is_disconnected; }
			
			void Attach( vfs::filehandle* target )  { its_tty = target; }
			
			vfs::filehandle* Next() const  { return its_tty.get(); }
			
			pid_t getpgrp() const  { return its_process_group_id; }
			
			void setpgrp( pid_t pgid );
			
			void IOCtl( unsigned long request, int* argp );
			
			void Disconnect();
	};
	
}

namespace relix
{
	
	vfs::filehandle_ptr new_terminal( const vfs::node& tty_file );
	
}

#endif

