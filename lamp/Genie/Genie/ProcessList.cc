/*
	ProcessList.cc
	--------------
*/

#include "Genie/ProcessList.hh"

// Standard C++
#include <vector>

// Standard C
#include <signal.h>

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/ptrvec.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// poseven
#include "poseven/types/errno_t.hh"

// relix-api
#include "relix/api/get_thread.hh"

// relix-kernel
#include "relix/config/mini.hh"
#include "relix/task/process.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	typedef std::vector< boost::intrusive_ptr< Process > > Process_Table;
	
	static Process_Table global_processes;
	
	
	const size_t max_n_tasks = CONFIG_MINI ? 128 : 1024;
	
	static pid_t global_last_pid = 0;
	
	Process* lookup_process( pid_t pid )
	{
		if ( size_t( pid ) < global_processes.size() )
		{
			if ( Process* process = global_processes[ pid ].get() )
			{
				if ( process->GetLifeStage() != kProcessReleased )
				{
					return process;
				}
			}
		}
		
		return NULL;
	}
	
	Process& get_process( pid_t pid )
	{
		if ( Process* process = lookup_process( pid ) )
		{
			return *process;
		}
		
		throw p7::errno_t( ESRCH );
	}
	
	static pid_t next_pid()
	{
		if ( ++global_last_pid >= global_processes.size() )
		{
			global_last_pid = 1;
		}
		
		pid_t last = global_last_pid - 1;
		
		Process_Table::iterator begin = global_processes.begin();
		Process_Table::iterator end   = global_processes.end();
		
		Process_Table::iterator it = std::find( begin + global_last_pid,
		                                        end,
		                                        Process_Table::value_type() );
		
		if ( it == end )
		{
			it = std::find( begin,
			                begin + global_last_pid,
			                Process_Table::value_type() );
			
			if ( it == begin + global_last_pid )
			{
				p7::throw_errno( EAGAIN );
			}
		}
		
		return global_last_pid = it - begin;
	}
	
	
	static pid_t cannot_self_terminate;
	
	static
	void destroy( pid_t tid )
	{
		boost::intrusive_ptr< Process >& slot = global_processes[ tid ];
		
		ASSERT( slot.get() != relix::gCurrentProcess );
		
		ASSERT( slot->GetLifeStage() == kProcessReleased );
		
		slot.reset();
	}
	
	void destroy_pending()
	{
		if ( cannot_self_terminate )
		{
			destroy( cannot_self_terminate );
			
			cannot_self_terminate = 0;
		}
	}
	
	void notify_reaper( Process* released )
	{
		ASSERT( released != NULL );
		
		destroy_pending();
		
		const pid_t tid = released->id();
		
		if ( released != relix::gCurrentProcess )
		{
			destroy( tid );
			return;
		}
		
		cannot_self_terminate = tid;
	}
	
	static Process& NewProcess( Process::RootProcess )
	{
		ASSERT( global_processes.empty() );
		
		global_processes.resize( max_n_tasks );
		
		const pid_t pid = next_pid();
		
		Process* new_process = new Process( Process::RootProcess() );
		
		global_processes[ pid ] = new_process;
		
		return *new_process;
	}
	
	Process& NewProcess( Process& parent )
	{
		const pid_t pid = next_pid();
		
		Process* new_process = new Process( parent, pid, pid );
		
		global_processes[ pid ] = new_process;
		
		return *new_process;
	}
	
	Process& NewThread( Process& caller )
	{
		const pid_t tid = next_pid();
		
		const pid_t pid  = caller.GetPID ();
		const pid_t ppid = caller.GetPPID();
		
		Process* new_process = new Process( caller, pid, tid );
		
		global_processes[ tid ] = new_process;
		
		return *new_process;
	}
	
	Process& GetInitProcess()
	{
		static Process& init = NewProcess( Process::RootProcess() );
		
		return init;
	}
	
	
	bool process_exists( pid_t pid )
	{
		return pid < global_processes.size()  &&  global_processes[ pid ].get();
	}
	
	void* for_each_process( void* (*f)( void*, pid_t, Process& ), void* param )
	{
		Process_Table::iterator begin = global_processes.begin();
		Process_Table::iterator end   = global_processes.end();
		
		for ( Process_Table::iterator it = begin;  it != end;  ++it )
		{
			if ( Process* process = it->get() )
			{
				if ( void* result = f( param, it - begin, *process ) )
				{
					return result;
				}
			}
		}
		
		return NULL;
	}
	
	void spawn_process( const char* path, const char* const* argv, const char* const* envp )
	{
		Process& parent = GetInitProcess();
		
		Process& child = NewProcess( parent );
		
		child.get_process().unshare_per_fork();
		
		try
		{
			(void) child.Exec( path, argv, envp );
		}
		catch ( ... )
		{
			remove_task( (relix::task*) &child );  // Bypass `protected`
			
			global_processes.at( child.gettid() ).reset();
		}
	}
	
	void spawn_process( const char* path, const char* const* argv )
	{
		const char* envp[] = { "PATH=/bin:/sbin:/usr/bin:/usr/sbin", NULL };
		
		spawn_process( path, argv, envp );
	}
	
	void spawn_process( const plus::string& program_args )
	{
		plus::var_string arg_string = program_args;
		
		vxo::UPtrVec< const char > args;
		
		const char* program_args_data = program_args.c_str();
		
		if ( const char* first_space = strchr( program_args_data, ' ' ) )
		{
			// Don't copy-on-write unless we have to
			
			char* p = arg_string.begin();
			
			char* space = p + (first_space - program_args_data);
			
			do
			{
				args.push_back( p );
				
				p = space;
				
				*p++ = '\0';
			}
			while (( space = strchr( p, ' ' ) ));
			
			args.push_back( p );
		}
		else
		{
			args.push_back( program_args_data );
		}
		
		args.push_back( NULL );
		
		char const *const *argv = &args[ 0 ];
		
		spawn_process( argv[ 0 ], argv );
	}
	
	static void* any_running( void*, pid_t pid, Process& process )
	{
		if ( process.id() > 1 )
		{
			return &process;
		}
		
		return NULL;
	}
	
	static void* send_sigterm_or_sigkill( void*, pid_t pid, Process& process )
	{
		// TODO:  Send SIGKILL on Command-Option-Q or such
		const int signo = SIGTERM;
		
		process.Raise( signo );
		
		return NULL;
	}
	
	static bool already_quitting = false;
	
	bool is_ready_to_exit()
	{
		relix::gCurrentProcess = NULL;
		
		destroy_pending();
		
		if ( !already_quitting )
		{
			for_each_process( &send_sigterm_or_sigkill );
			
			already_quitting = true;
		}
		
		return for_each_process( &any_running ) == NULL;
	}
	
}

namespace relix
{
	
	thread& get_thread( int tid )
	{
		return Genie::get_process( tid );
	}
	
}
