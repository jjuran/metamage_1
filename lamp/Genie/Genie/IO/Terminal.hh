/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// POSIX
#include "errno.h"

// plus
#include "plus/string.hh"

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/IO/IOHandle_Cast.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	const pid_t no_pgid = 0x7fffffff;
	
	
	class TerminalHandle : public IOHandle
	{
		private:
			const plus::string  itsTTYName;
			IOPtr               itsTTY;
			pid_t               its_process_group_id;
			
			IOHandle* Next() const  { return itsTTY.get(); }
		
		public:
			TerminalHandle( const plus::string& tty_name );
			
			~TerminalHandle();
			
			bool IsTerminal() const  { return true; }
			
			void Attach( vfs::filehandle* target )  { itsTTY = target; }
			
			FSTreePtr GetFile();
			
			pid_t getpgrp() const  { return its_process_group_id; }
			
			void setpgrp( pid_t pgid )  { its_process_group_id = pgid; }
			
			void IOCtl( unsigned long request, int* argp );
			
			void Disconnect();
			
			const plus::string& TTYName() const  { return itsTTYName; }
	};
	
	template <> struct IOHandle_Downcast_Traits< TerminalHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsTerminal; }
		
		static int GetError( IOHandle& )  { return ENOTTY; }
	};
	
}

#endif

