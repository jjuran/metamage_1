/*	===========
 *	Terminal.cc
 *	===========
 */

#include "Genie/IO/Terminal.hh"

// Standard C
#include <signal.h>

// POSIX
#include <fcntl.h>
#include "sys/ttycom.h"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/terminal_method_set.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix
#include "relix/signal/signal_process_group.hh"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static void terminal_hangup( vfs::filehandle* that )
	{
		TerminalHandle& terminal = static_cast< TerminalHandle& >( *that );
		
		terminal.Disconnect();
	}
	
	static const vfs::terminal_method_set terminal_methods =
	{
		&terminal_hangup
	};
	
	static const vfs::filehandle_method_set filehandle_methods =
	{
		NULL,
		NULL,
		&terminal_methods
	};
	
	TerminalHandle::TerminalHandle( const plus::string& tty_name )
	:
		vfs::filehandle     ( O_RDWR, &filehandle_methods ),
		itsTTYName          ( tty_name ),
		its_process_group_id( no_pgid  )
	{
	}
	
	TerminalHandle::~TerminalHandle()
	{
	}
	
	FSTreePtr TerminalHandle::GetFile()
	{
		return vfs::resolve_absolute_path( itsTTYName );
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
		
		relix::session& process_session = current.GetProcessGroup().get_session();
		
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
		if ( StreamHandle* tty = IOHandle_Cast< StreamHandle >( Next() ) )
		{
			tty->Disconnect();
		}
		
		const pid_t pgid = vfs::getpgrp( *this );
		
		relix::signal_process_group( SIGHUP, pgid );
	}
	
}

