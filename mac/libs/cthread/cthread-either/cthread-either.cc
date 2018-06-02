/*
	cthread-either.cc
	-----------------
*/

#include "cthread-either.hh"

// iota
#include "iota/dummy.hh"

#if CTHREAD_EITHER
#include "cthread-custom.hh"
#include "cthread-system.hh"
#endif

// mac-sys-utils
#include "mac_sys/has/ThreadManager.hh"


namespace cthread
{
	
	void thread_yield()
	{
		either::yield_to_any_thread();
	}
	
}

#if CTHREAD_EITHER

namespace cthread {
namespace either  {
	
	static const bool has_ThreadManager = mac::sys::has_ThreadManager();
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size )
	{
		return has_ThreadManager ? system::create_thread( pb, stack_size )
		                         : custom::create_thread( pb, stack_size );
	}
	
	void destroy_thread( thread_id id )
	{
		if ( has_ThreadManager )
		{
			system::destroy_thread( id );
		}
		else
		{
			custom::destroy_thread( id );
		}
	}
	
	thread_id current_thread()
	{
		return has_ThreadManager ? system::current_thread()
		                         : custom::current_thread();
	}
	
	unsigned long current_thread_stack_space()
	{
		return has_ThreadManager ? system::current_thread_stack_space()
		                         : custom::current_thread_stack_space();
	}
	
	bool is_thread_stopped( thread_id id )
	{
		return has_ThreadManager ? system::is_thread_stopped( id )
		                         : custom::is_thread_stopped( id );
	}
	
	void stop_thread( thread_id id )
	{
		if ( has_ThreadManager )
		{
			system::stop_thread( id );
		}
		else
		{
			custom::stop_thread( id );
		}
	}
	
	void wake_thread( thread_id id )
	{
		if ( has_ThreadManager )
		{
			system::wake_thread( id );
		}
		else
		{
			custom::wake_thread( id );
		}
	}
	
	bool woken_thread( thread_id id )
	{
		if ( has_ThreadManager )
		{
			return system::woken_thread( id );
		}
		else
		{
			return custom::woken_thread( id );
		}
	}
	
	void yield_to_any_thread()
	{
		if ( has_ThreadManager )
		{
			system::yield_to_any_thread();
		}
		else
		{
			custom::yield_to_any_thread();
		}
	}
	
	void yield_to_thread( thread_id id )
	{
		if ( has_ThreadManager )
		{
			system::yield_to_thread( id );
		}
		else
		{
			custom::yield_to_thread( id );
		}
	}
	
}
}

#endif
