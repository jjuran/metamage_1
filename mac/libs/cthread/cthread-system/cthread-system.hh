/*
	cthread-system.hh
	-----------------
*/

#ifndef CTHREADSYSTEM_HH
#define CTHREADSYSTEM_HH


namespace cthread
{
	
	struct parameter_block;
	
	typedef unsigned long thread_id;
	
namespace system
{
	
	using cthread::parameter_block;
	
	void yield_to_any_thread();
	
	unsigned long current_thread_stack_space();
	
	thread_id current_thread();
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size );
	
	void destroy_thread( thread_id id );
	
	bool is_thread_stopped( thread_id id );
	
	void stop_thread( thread_id id );
	void wake_thread( thread_id id );
	
	short stop_thread_nothrow( thread_id id );
	
	bool woken_thread( thread_id id );
	
	void yield_to_thread( thread_id id );
	
}
}

#endif
