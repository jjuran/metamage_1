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
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/terminal_method_set.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/poll.hh"
#include "vfs/filehandle/primitives/read.hh"
#include "vfs/filehandle/primitives/write.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/signal/signal_process_group.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static vfs::filehandle& get_tty( vfs::filehandle* that )
	{
		TerminalHandle& terminal = static_cast< TerminalHandle& >( *that );
		
		if ( terminal.disconnected() )
		{
			p7::throw_errno( EIO );
		}
		
		vfs::filehandle* tty = terminal.Next();
		
		if ( tty == NULL )
		{
			p7::throw_errno( ENXIO );
		}
		
		return *tty;
	}
	
	static unsigned terminal_poll( vfs::filehandle* that )
	{
		return poll( get_tty( that ) );
	}
	
	static ssize_t terminal_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		return read( get_tty( that ), buffer, n );
	}
	
	static ssize_t terminal_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return write( get_tty( that ), buffer, n );
	}
	
	static const vfs::stream_method_set terminal_stream_methods =
	{
		&terminal_poll,
		&terminal_read,
		&terminal_write,
	};
	
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
		&terminal_stream_methods,
		NULL,
		&terminal_methods
	};
	
	TerminalHandle::TerminalHandle( const vfs::node& tty_file )
	:
		vfs::filehandle     ( &tty_file, O_RDWR, &filehandle_methods ),
		its_process_group_id( no_pgid  ),
		it_is_disconnected  ( false )
	{
	}
	
	TerminalHandle::~TerminalHandle()
	{
	}
	
	static void CheckControllingTerminal( const vfs::filehandle* ctty, const TerminalHandle& tty )
	{
		if ( ctty != &tty )
		{
			p7::throw_errno( ENOTTY );
		}
	}
	
	void TerminalHandle::IOCtl( unsigned long request, int* argp )
	{
		relix::process& current = relix::current_process();
		
		relix::process_group& process_group = current.get_process_group();
		
		relix::session& process_session = process_group.get_session();
		
		vfs::filehandle* ctty = process_session.get_ctty().get();
		
		switch ( request )
		{
			case TIOCGPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( ctty, *this );
				
				*argp = its_process_group_id;
				
				break;
			
			case TIOCSPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( ctty, *this );
				
				{
					// If the terminal has an existing foreground process group,
					// it must be in the same session as the calling process.
					if ( its_process_group_id == no_pgid  ||  &FindProcessGroup( its_process_group_id )->get_session() == &process_session )
					{
						// This must be the caller's controlling terminal.
						if ( ctty == this )
						{
							setpgrp( GetProcessGroupInSession( *argp, process_session )->id() );
						}
					}
					
					p7::throw_errno( ENOTTY );
				}
				
				break;
			
			case TIOCSCTTY:
				if ( process_session.id() != current.id() )
				{
					// not a session leader
					p7::throw_errno( EPERM );
				}
				
				if ( ctty != NULL )
				{
					// already has a controlling terminal
					p7::throw_errno( EPERM );
				}
				
				// Check that we're not the controlling tty of another session
				
				this->setpgrp( process_group.id() );
				
				process_session.set_ctty( *this );
				break;
			
			default:
				IOHandle::IOCtl( request, argp );
				break;
		};
	}
	
	void TerminalHandle::Disconnect()
	{
		it_is_disconnected = true;
		
		const pid_t pgid = vfs::getpgrp( *this );
		
		relix::signal_process_group( SIGHUP, pgid );
	}
	
}

