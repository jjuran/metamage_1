/*	===========
 *	Terminal.cc
 *	===========
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
	
	namespace p7 = poseven;
	
	TerminalHandle::~TerminalHandle()
	{
	}
	
	FSTreePtr TerminalHandle::GetFile() const
	{
		return ResolvePathname( itsTTYName, FSRoot() );
	}
	
	static void CheckControllingTerminal( const Process& process, const TerminalHandle& tty )
	{
		const boost::shared_ptr< IOHandle >& process_ctty = process.ControllingTerminal();
		
		if ( process_ctty.get() != &tty )
		{
			p7::throw_errno( ENOTTY );
		}
	}
	
	void TerminalHandle::IOCtl( unsigned long request, int* argp )
	{
		Process& current = CurrentProcess();
		
		const boost::shared_ptr< Session >& process_session = current.GetProcessGroup()->GetSession();
		
		switch ( request )
		{
			case TIOCGPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				*argp = !GetProcessGroup().expired() ? GetProcessGroup().lock()->ID()
				                                     : 0x7fffffff;
				
				break;
			
			case TIOCSPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( current, *this );
				
				{
					boost::shared_ptr< Session > terminal_session = !GetProcessGroup().expired() ? GetProcessGroup().lock()->GetSession()
					                                                                             : process_session;
					
					// Since this is the process' controlling terminal, they should share a session
					ASSERT( terminal_session.get() == process_session.get() );
					
					if ( process_session->GetControllingTerminal().get() != this )
					{
						p7::throw_errno( ENOTTY );
					}
					
					SetProcessGroup( GetProcessGroupInSession( *argp, terminal_session ) );
				}
				
				break;
			
			case TIOCSCTTY:
				if ( process_session->ID() != current.GetPID() )
				{
					// not a session leader
					p7::throw_errno( EPERM );
				}
				
				if ( process_session->GetControllingTerminal().get() != NULL )
				{
					// already has a controlling terminal
					p7::throw_errno( EPERM );
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
				IOHandle::IOCtl( request, argp );
				break;
		};
	}
	
}

