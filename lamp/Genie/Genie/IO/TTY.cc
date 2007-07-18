/*	======
 *	TTY.cc
 *	======
 */

#include "Genie/IO/TTY.hh"

// Standard C
#include <signal.h>

// POSIX
#include "sys/ttycom.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	FSTreePtr TTYHandle::GetFile() const
	{
		return ResolvePathname( ttyName, FSRoot() );
	}
	
	static void CheckControllingTerminal( const Process& process, const TTYHandle& tty )
	{
		const boost::shared_ptr< IOHandle >& process_ctty = process.ControllingTerminal();
		
		if ( process_ctty.get() != &tty )
		{
			P7::ThrowErrno( ENOTTY );
		}
	}
	
	void TTYHandle::IOCtl( unsigned long request, int* argp )
	{
		Process& current = CurrentProcess();
		
		const boost::shared_ptr< Session >& process_session = current.GetProcessGroup()->GetSession();
		
		switch ( request )
		{
			case TIOCGPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				ASSERT( !GetProcessGroup().expired() );
				
				*argp = GetProcessGroup().lock()->ID();
				
				break;
			
			case TIOCSPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				{
					boost::shared_ptr< Session > terminal_session = GetProcessGroup().lock()->GetSession();
					
					// Since this is the process' controlling terminal, they should share a session
					ASSERT( terminal_session.get() == process_session.get() );
					
					if ( process_session->GetControllingTerminal().get() != this )
					{
						P7::ThrowErrno( ENOTTY );
					}
					
					SetProcessGroup( GetProcessGroupInSession( *argp, terminal_session ) );
				}
				
				break;
			
			case TIOCSCTTY:
				if ( process_session->ID() != current.GetPID() )
				{
					// not a session leader
					P7::ThrowErrno( EPERM );
				}
				
				if ( process_session->GetControllingTerminal().get() != NULL )
				{
					// already has a controlling terminal
					P7::ThrowErrno( EPERM );
				}
				
				// Check that we're not the controlling tty of another session
				
				SetProcessGroup( current.GetProcessGroup() );
				
				process_session->SetControllingTerminal( shared_from_this() );
				break;
			
			case TIOCNOTTY:
				if ( process_session->ID() != current.GetPID() )
				{
					// not a session leader
					//current.GiveUpControllingTerminal();
				}
				else
				{
					process_session->SetControllingTerminal( boost::shared_ptr< IOHandle >() );
					
					const ProcessGroup* group = GetProcessGroup().lock().get();
					
					ASSERT( group != NULL );
					
					SendSignalToProcessGroup( SIGHUP,  *group );
					SendSignalToProcessGroup( SIGCONT, *group );
				}
				break;
			
			default:
				FileHandle::IOCtl( request, argp );
				break;
		};
	}
	
}

