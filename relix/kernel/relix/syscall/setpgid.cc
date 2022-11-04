/*
	setpgid.cc
	----------
*/

#include "relix/syscall/setpgid.hh"

// Standard C
#include <errno.h>

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/api/get_process.hh"
#include "relix/api/get_session_process_group.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"


namespace relix
{
	
	int setpgid( int pid, int pgid )
	{
		try
		{
			if ( pgid < 0 )
			{
				return set_errno( EINVAL );
			}
			
			process& current = current_process();
			
			process& target = pid != 0 ? get_process( pid )
			                           : current;
			
			const int current_pid = current.id();
			const int target_pid  = target .id();
			
			session& target_session = target.get_process_group().get_session();
			
			const int target_sid = target_session.id();
			
			bool target_is_self = pid == 0  ||  target_pid == current_pid;
			
			if ( target_is_self )
			{
				// A session-leading child is in a different session, which we test for
				
				if ( target_sid == target_pid )
				{
					return set_errno( EPERM );  // target is a session leader
				}
			}
			else
			{
				bool target_is_child = current_pid == target.getppid();
				
				if ( !target_is_child )
				{
					return set_errno( ESRCH );  // target is not self or a child
				}
				
				if ( &target.get_process_image() != &current.get_process_image() )
				{
					return set_errno( EACCES );  // child already execve'd
				}
				
				const int current_sid = current.get_process_group().get_session().id();
				
				if ( current_sid != target_sid )
				{
					return set_errno( EPERM );  // child in different session
				}
			}
			
			
			if ( pgid == 0 )
			{
				pgid = target_pid;
			}
			
			target.set_process_group( *get_session_process_group( pgid, target_session ) );
			
			return 0;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}
