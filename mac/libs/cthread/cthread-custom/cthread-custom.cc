/*
	cthread-custom.cc
	-----------------
*/

#include "cthread-custom.hh"

// Standard C
#include <stdlib.h>

// cthread
#include "cthread/parameter_block.hh"

// debug
#include "debug/assert.hh"

// cthread-custom
#include "circular_queue.hh"


#ifdef __MC68K__
#define AT( addr )  : addr
#define STATIC static
#else
#define AT( addr )
#define STATIC extern  // Avoid unneeded static initialization
#endif

#ifndef NULL
#define NULL  0
#endif

const void* StkLowPt   AT( 0x110 );
const void* HeapEnd    AT( 0x114 );
const void* ApplLimit  AT( 0x130 );
const void* HiHeapMark AT( 0xBAE );


namespace cthread {
namespace custom  {
	
	static const void* saved_StkLowPt;
	static const void* saved_HeapEnd;
	static const void* saved_ApplLimit;
	static const void* saved_HiHeapMark;
	
	struct machine_state
	{
		long   d3_7[ 1 + 7 - 3 ];  // [5]
		void*  pc;
		void*  a2_4[ 1 + 4 - 2 ];  // [3]
		void*  a6;
		void*  a7;
	};
	
	enum task_schedule
	{
		Task_stopped,
		Task_sleeping,
		Task_ready,
		Task_running,
		Task_ended = -1,
	};
	
	struct thread_task : queue_element
	{
		parameter_block*  pb;
		switch_proc       switch_in;
		switch_proc       switch_out;
		task_schedule     schedule;
		machine_state     state;
	};
	
	STATIC circular_queue task_queue;
	
	STATIC thread_task* main_task;
	
	static
	thread_task* add_main_task()
	{
	#if __A5__
		
		saved_StkLowPt   = StkLowPt;
		saved_HeapEnd    = HeapEnd;
		saved_ApplLimit  = ApplLimit;
		saved_HiHeapMark = HiHeapMark;
		
	#endif
		
		static thread_task task;
		
		task.schedule = Task_running;
		
		task_queue.reset( &task );
		
		return &task;
	}
	
	static
	void init_tasks_idempotent()
	{
		if ( main_task == NULL )
		{
			main_task = add_main_task();
		}
	}
	
	static inline
	thread_task* current_task()
	{
		return (thread_task*) task_queue.tail();
	}
	
	static inline
	void destroy_task( thread_task* task )
	{
		free( task );
	}
	
#ifdef __MC68K__
	
	static
	asm void task_switch( machine_state* a : __A0, machine_state* b )
	{
		MOVEA.L  (A7)+,             A1    // pop return address into A1
		
		MOVEM.L  D3-D7/A1-A4/A6-A7, (A0)  // save old task's state
		
		MOVEA.L  (A7), A0
		MOVEM.L  (A0), D3-D7/A1-A4/A6-A7  // set up new task's state
		
		JMP      (A1)
	}
	
#else
	
	static
	void task_switch( machine_state* a, machine_state* b )
	{
	}
	
#endif
	
	static
	void prepare_task( thread_task* task )
	{
	#if __A5__
		
		/*
			Credit to Ari Halberstadt, whose Thread Library provided clues on
			which low memory globals need to be zapped to keep QuickDraw happy.
		*/
		
		if ( task->pb != NULL )
		{
			const void* limit = task->pb->stack_limit;
			
			// StkLowPt was already cleared in suspend_task().
			
			HeapEnd    = limit;
			ApplLimit  = limit;
			HiHeapMark = limit;
		}
		else
		{
			StkLowPt   = saved_StkLowPt;
			HeapEnd    = saved_HeapEnd;
			ApplLimit  = saved_ApplLimit;
			HiHeapMark = saved_HiHeapMark;
		}
		
	#endif
	}
	
	static
	void suspend_task( task_schedule suspended, thread_task* a, thread_task* b )
	{
		a->schedule = suspended;
		b->schedule = Task_running;
		
		if ( switch_proc f = a->switch_out )
		{
			f( a->pb->param );
		}
		
		/*
			Switch from task a to task b.
			This blocks the current task until it's switched back in.
		*/
		
	#if __A5__
		
		// Clear StkLowPt BEFORE switching stacks.
		StkLowPt = NULL;
		
	#endif
		
		task_switch( &a->state, &b->state );
		
		prepare_task( a );
		
		if ( switch_proc f = a->switch_in )
		{
			f( a->pb->param );
		}
	}
	
	static inline
	bool ended( const thread_task* task )
	{
		ASSERT( task != NULL );
		
		return task->schedule < 0;
	}
	
	static
	thread_task* next_task()
	{
		while ( ended( (thread_task*) task_queue.head() ) )
		{
			destroy_task( (thread_task*) task_queue.behead() );
		}
		
		return (thread_task*) task_queue.next();
	}
	
	static
	thread_task* next_runnable_task()
	{
		while ( next_task()->schedule == Task_stopped )  continue;
		
		return current_task();
	}
	
	static
	void select_next_task( thread_task* task )
	{
		task_queue.select( task );
	}
	
	static inline
	thread_id get_thread_id( thread_task* task )
	{
		return (thread_id) task;
	}
	
	static inline
	thread_task* task_from_id( thread_id id )
	{
		return (thread_task*) id;
	}
	
	static
	thread_task* verified( thread_task* task )
	{
		return task_queue.contains( task ) ? task : NULL;
	}
	
	thread_id current_thread()
	{
		init_tasks_idempotent();
		
		return get_thread_id( current_task() );
	}
	
#ifdef __MC68K__
	
	static inline
	asm void* get_SP()
	{
		MOVEA.L  A7,A0
	}
	
#else
	
	static inline
	void* get_SP()
	{
		return NULL;
	}
	
#endif
	
	unsigned long current_thread_stack_space()
	{
		thread_task* task = current_task();
		
		if ( ! task->pb )
		{
			return 0;
		}
		
		const void* limit = task->pb->stack_limit;
		const void* point = get_SP();
		
		return (char*) point - (char*) limit;  // The stack grows downward.
	}
	
	bool is_thread_stopped( thread_id id )
	{
		const thread_task* that = task_from_id( id );
		
		return that->schedule == Task_stopped;
	}
	
	void stop_thread( thread_id id )
	{
		thread_task* task = current_task();
		thread_task* that = task_from_id( id );
		
		if ( that == task )
		{
			thread_task* next = next_runnable_task();
			
			if ( that == next )
			{
				// No other runnable threads
				
				return;  // throw?
			}
			
			suspend_task( Task_stopped, task, next );
			
			return;
		}
		
		that->schedule = Task_stopped;
	}
	
	void wake_thread( thread_id id )
	{
		thread_task* that = task_from_id( id );
		
		if ( that != current_task() )
		{
			that->schedule = Task_ready;
		}
	}
	
	bool woken_thread( thread_id id )
	{
		if ( thread_task* that = verified( task_from_id( id ) ) )
		{
			if ( that->schedule == Task_stopped )
			{
				that->schedule = Task_ready;
			}
			
			return true;
		}
		
		return false;
	}
	
	void yield_to_any_thread()
	{
		if ( ! main_task )
		{
			return;
		}
		
		thread_task* task = current_task();
		thread_task* next = next_runnable_task();
		
		if ( task != next )
		{
			suspend_task( Task_sleeping, task, next );
		}
	}
	
	void yield_to_thread( thread_id id )
	{
		thread_task* task = current_task();
		thread_task* next = task_from_id( id );
		
		if ( task != next )
		{
			select_next_task( next );
			
			suspend_task( Task_sleeping, task, next );
		}
	}
	
	static
	void end_of_task()
	{
		thread_task* task = current_task();
		thread_task* next = next_runnable_task();
		
		ASSERT( task != next );
		
		suspend_task( Task_ended, task, next );
	}
	
	static
	void thread_entry( void* param )
	{
		parameter_block& pb = *(parameter_block*) param;
		
		thread_task* task = current_task();
		
		prepare_task( task );
		
		if ( switch_proc f = task->switch_in )
		{
			f( task->pb->param );
		}
		
		try
		{
			pb.start( pb.param, pb.stack_bottom, pb.stack_limit );
		}
		catch ( ... )
		{
		}
	}
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size )
	{
		init_tasks_idempotent();
		
		const unsigned long minimum_size = 32768;
		
		if ( stack_size < minimum_size )
		{
			stack_size = minimum_size;
		}
		
		void* stack = malloc( stack_size );
		
		if ( stack == NULL )
		{
			return thread_id();
		}
		
		thread_task* task_ptr = (thread_task*) stack;
		thread_task& task     = *task_ptr;
		
		void* base = (char*) stack + stack_size;
		
		void** fp = (void**) base;
		
		*--fp = 0;  // null return address
		*--fp = 0;  // null frame pointer backlink
		
		pb.stack_bottom = fp;
		pb.stack_limit  = task_ptr + 1;  // Leave room for a header?
		
		void** sp = fp;
		
		*--sp = &pb;
		*--sp = (void*) &end_of_task;
		
		task.pb = &pb;
		
		task.switch_in  = pb.switch_in;
		task.switch_out = pb.switch_out;
		
		task.schedule = Task_sleeping;
		
		task.state.pc = (void*) thread_entry;
		task.state.a6 = fp;
		task.state.a7 = sp;
		
		task_queue.prepend( &task );
		
		return (thread_id) &task;
	}
	
	void destroy_thread( thread_id id )
	{
		thread_task* that = task_from_id( id );
		
		ASSERT( that != main_task );
		
		if ( that == current_task() )
		{
			end_of_task();  // doesn't return
		}
		
		that->schedule = Task_ended;
	}
	
}
}
