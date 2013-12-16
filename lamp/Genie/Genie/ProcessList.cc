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

// Boost
#include <boost/intrusive_ptr.hpp>

// poseven
#include "poseven/types/errno_t.hh"

// MacFeatures
#include "MacFeatures/Threads.hh"

// relix-api
#include "relix/api/get_thread.hh"

// relix-kernel
#include "relix/config/mini.hh"
#include "relix/api/os_thread_api.hh"
#include "relix/api/os_thread_box.hh"

// Genie
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
	
	
	static bool reaper_must_run = false;
	
	void notify_reaper()
	{
		reaper_must_run = true;
	}
	
	static void* reap_process( void*, pid_t pid, Process& process )
	{
		if ( process.GetLifeStage() == kProcessReleased )
		{
			boost::intrusive_ptr< Process >().swap( global_processes[ pid ] );
		}
		
		return NULL;
	}
	
	void ReaperThreadEntry();
	
	static void* reaper_thread_start( void* param, const void* bottom, const void* limit )
	{
		while ( true )
		{
			if ( reaper_must_run )
			{
				reaper_must_run = false;
				
				for_each_process( &reap_process );
			}
			
			relix::os_thread_yield();
		}
		
		return NULL;
	}
	
	static Process& NewProcess( Process::RootProcess )
	{
		static const bool has_ThreadManager = MacFeatures::Has_Threads();
		
		if ( !has_ThreadManager )
		{
			p7::throw_errno( ENOSYS );
		}
		
		static relix::os_thread_box reaper = relix::new_os_thread( &reaper_thread_start, NULL, 0 );
		
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
		
		child.unshare_fs_info();
		child.unshare_files();
		child.unshare_signal_handlers();
		
		try
		{
			(void) child.Exec( path, argv, envp );
		}
		catch ( ... )
		{
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
		
		std::vector< const char* > args;
		
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
	
	static void* send_sigterm( void*, pid_t pid, Process& process )
	{
		process.Raise( SIGTERM );
		
		return NULL;
	}
	
	static bool already_quitting = false;
	
	bool is_ready_to_exit()
	{
		if ( !already_quitting )
		{
			for_each_process( &send_sigterm );
			
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

