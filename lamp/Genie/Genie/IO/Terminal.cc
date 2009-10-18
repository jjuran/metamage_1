/*	===========
 *	Terminal.cc
 *	===========
 */

#include "Genie/IO/TTY.hh"

// Standard C
#include <signal.h>

// POSIX
#include "sys/ttycom.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"


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
					// If the terminal has an existing foreground process group,
					// it must be in the same session as the calling process.
					if ( GetProcessGroup().expired()  ||  GetProcessGroup().lock()->GetSession().get() == process_session.get() )
					{
						// This must be the caller's controlling terminal.
						if ( process_session->GetControllingTerminal().get() == this )
						{
							SetProcessGroup( GetProcessGroupInSession( *argp, process_session ) );
						}
					}
					
					p7::throw_errno( ENOTTY );
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
		
		if ( !GetProcessGroup().expired() )
		{
			SendSignalToProcessGroup( SIGHUP, *GetProcessGroup().lock() );
		}
	}
	
}

