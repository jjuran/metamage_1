/*	===========
 *	Terminal.cc
 *	===========
 */

#include "Genie/IO/Terminal.hh"

// Standard C
#include <signal.h>

// POSIX
#include "sys/ttycom.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	TerminalHandle::~TerminalHandle()
	{
	}
	
	FSTreePtr TerminalHandle::GetFile()
	{
		return ResolveAbsolutePath( itsTTYName );
	}
	
	static void CheckControllingTerminal( const Process& process, const TerminalHandle& tty )
	{
		const IOPtr& process_ctty = process.ControllingTerminal();
		
		if ( process_ctty.get() != &tty )
		{
			p7::throw_errno( ENOTTY );
		}
	}
	
	void TerminalHandle::IOCtl( unsigned long request, int* argp )
	{
		Process& current = CurrentProcess();
		
		Session& process_session = current.GetProcessGroup().get_session();
		
		switch ( request )
		{
			case TIOCGPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				*argp = its_process_group_id;
				
				break;
			
			case TIOCSPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				{
					// If the terminal has an existing foreground process group,
					// it must be in the same session as the calling process.
					if ( its_process_group_id == no_pgid  ||  &FindProcessGroup( its_process_group_id )->get_session() == &process_session )
					{
						// This must be the caller's controlling terminal.
						if ( process_session.get_ctty().get() == this )
						{
							setpgrp( GetProcessGroupInSession( *argp, process_session )->id() );
						}
					}
					
					p7::throw_errno( ENOTTY );
				}
				
				break;
			
			case TIOCSCTTY:
				if ( process_session.id() != current.GetPID() )
				{
					// not a session leader
					p7::throw_errno( EPERM );
				}
				
				if ( process_session.get_ctty().get() != NULL )
				{
					// already has a controlling terminal
					p7::throw_errno( EPERM );
				}
				
				// Check that we're not the controlling tty of another session
				
				this->setpgrp( current.GetPGID() );
				
				process_session.set_ctty( *this );
				break;
			
			default:
				IOHandle::IOCtl( request, argp );
				break;
		};
	}
	
	void TerminalHandle::Disconnect()
	{
		itIsDisconnected = true;
		
		if ( StreamHandle* tty = IOHandle_Cast< StreamHandle >( Next() ) )
		{
			tty->Disconnect();
		}
		
		send_signal_to_foreground_process_group_of_terminal( SIGHUP, *this );
	}
	
	void send_signal_to_foreground_process_group_of_terminal( int signo, const TerminalHandle& h )
	{
		if ( h.getpgrp() != no_pgid )
		{
			SendSignalToProcessGroup( signo, h.getpgrp() );
		}
	}
	
}

