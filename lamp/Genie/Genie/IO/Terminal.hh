/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	const pid_t no_pgid = 0x7fffffff;
	
	
	class TerminalHandle : public vfs::filehandle
	{
		private:
			const plus::string   itsTTYName;
			vfs::filehandle_ptr  its_tty;
			pid_t                its_process_group_id;
			
			vfs::filehandle* Next() const  { return its_tty.get(); }
		
		public:
			TerminalHandle( const plus::string& tty_name );
			
			~TerminalHandle();
			
			void Attach( vfs::filehandle* target )  { its_tty = target; }
			
			vfs::node_ptr GetFile();
			
			pid_t getpgrp() const  { return its_process_group_id; }
			
			void setpgrp( pid_t pgid )  { its_process_group_id = pgid; }
			
			void IOCtl( unsigned long request, int* argp );
			
			void Disconnect();
	};
	
}

#endif

