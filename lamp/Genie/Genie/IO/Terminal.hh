/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// POSIX
#include "errno.h"
#include "fcntl.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	const pid_t no_pgid = 0x7fffffff;
	
	
	class TerminalHandle : public IOHandle
	{
		private:
			const plus::string             itsTTYName;
			boost::shared_ptr< IOHandle >  itsTTY;
			pid_t                          its_process_group_id;
			bool                           itIsDisconnected;
			
			IOHandle* Next() const  { return itsTTY.get(); }
		
		public:
			TerminalHandle( const plus::string& ttyName )
			:
				IOHandle( O_RDWR ),
				itsTTYName          ( ttyName ),
				its_process_group_id( no_pgid ),
				itIsDisconnected    ( false   )
			{
			}
			
			~TerminalHandle();
			
			bool IsTerminal() const  { return true; }
			
			void Attach( const boost::shared_ptr< IOHandle >& target )  { itsTTY = target; }
			
			FSTreePtr GetFile();
			
			pid_t getpgrp() const  { return its_process_group_id; }
			
			void setpgrp( pid_t pgid )  { its_process_group_id = pgid; }
			
			void IOCtl( unsigned long request, int* argp );
			
			bool IsDisconnected() const  { return itIsDisconnected; }
			
			void Disconnect();
			
			const plus::string& TTYName() const  { return itsTTYName; }
	};
	
	template <> struct IOHandle_Downcast_Traits< TerminalHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsTerminal; }
		
		static int GetError( IOHandle& )  { return ENOTTY; }
	};
	
	void send_signal_to_foreground_process_group_of_terminal( int signo, const TerminalHandle& h );
	
}

#endif

