/*
	terminal.cc
	-----------
*/

#include "relix/fs/terminal.hh"

// Standard C
#include <signal.h>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/terminal_method_set.hh"
#include "vfs/filehandle/primitives/ioctl.hh"
#include "vfs/filehandle/primitives/poll.hh"
#include "vfs/filehandle/primitives/read.hh"
#include "vfs/filehandle/primitives/setpgrp.hh"
#include "vfs/filehandle/primitives/write.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/api/get_process_group.hh"
#include "relix/api/get_session_process_group.hh"
#include "relix/signal/signal_process_group.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	static vfs::filehandle& get_tty( vfs::filehandle* that )
	{
		terminal_extra& extra = *(terminal_extra*) that->extra();
		
		vfs::filehandle* tty = extra.tty;
		
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
	
	static void CheckControllingTerminal( const vfs::filehandle* ctty, const vfs::filehandle& tty )
	{
		if ( ctty != &tty )
		{
			p7::throw_errno( ENOTTY );
		}
	}
	
	static bool session_controls_pgrp( const session& session, pid_t pgid )
	{
		if ( pgid == no_pgid )
		{
			return true;
		}
		
		process_group* pgrp = get_process_group( pgid );
		
		return pgrp  &&  &pgrp->get_session() == &session;
	}
	
	static void terminal_ioctl( vfs::filehandle* that, unsigned long request, int* argp )
	{
		terminal_extra& extra = *(terminal_extra*) that->extra();
		
		process& current = current_process();
		
		process_group& process_group = current.get_process_group();
		
		session& process_session = process_group.get_session();
		
		vfs::filehandle* ctty = process_session.get_ctty().get();
		
		switch ( request )
		{
			case TIOCGPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( ctty, *that );
				
				*argp = extra.pgid;
				
				break;
			
			case TIOCSPGRP:
				ASSERT( argp != NULL );
				
				CheckControllingTerminal( ctty, *that );
				
				// If the terminal has an existing foreground process group,
				// it must be in the same session as the calling process.
				if ( ! session_controls_pgrp( process_session, extra.pgid ) )
				{
					p7::throw_errno( ENOTTY );
				}
				
				setpgrp( *that, get_session_process_group( *argp, process_session )->id() );
				
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
				
				setpgrp( *that, process_group.id() );
				
				process_session.set_ctty( *that );
				break;
			
			default:
				if ( extra.tty == NULL )
				{
					p7::throw_errno( EINVAL );
				}
				
				ioctl( *extra.tty, request, argp );
				
				break;
		};
	}
	
	static void terminal_conjoin( vfs::filehandle& that, vfs::filehandle& target )
	{
		terminal_extra& extra = *(terminal_extra*) that.extra();
		
		intrusive_ptr_add_ref( &target );
		
		if ( extra.tty )
		{
			intrusive_ptr_release( extra.tty );
		}
		
		extra.tty = &target;
	}
	
	static void terminal_hangup( vfs::filehandle* that )
	{
		terminal_extra& extra = *(terminal_extra*) that->extra();
		
		extra.disconnected = true;
		
		signal_process_group( SIGHUP, extra.pgid );
	}
	
	
	static const vfs::general_method_set terminal_general_methods =
	{
		NULL,
		&terminal_ioctl,
		NULL,
		&terminal_conjoin,
	};
	
	static const vfs::terminal_method_set terminal_methods =
	{
		&terminal_hangup
	};
	
	static const vfs::filehandle_method_set filehandle_methods =
	{
		NULL,
		NULL,
		&terminal_stream_methods,
		&terminal_general_methods,
		&terminal_methods
	};
	
	static void destroy_terminal( vfs::filehandle* that )
	{
		terminal_extra& extra = *(terminal_extra*) that->extra();
		
		if ( extra.tty )
		{
			intrusive_ptr_release( extra.tty );
		}
	}
	
	vfs::filehandle_ptr new_terminal( const vfs::node& tty_file )
	{
		vfs::filehandle* result = new vfs::filehandle( &tty_file,
		                                               O_RDWR,
		                                               &filehandle_methods,
		                                               sizeof (terminal_extra),
		                                               &destroy_terminal );
		
		terminal_extra& extra = *(terminal_extra*) result->extra();
		
		extra.tty          = NULL;
		extra.pgid         = no_pgid;
		extra.disconnected = false;
		
		return result;
	}
	
}
