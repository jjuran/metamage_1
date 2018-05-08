/*
	thread.hh
	---------
*/

#ifndef RELIX_TASK_THREAD_HH
#define RELIX_TASK_THREAD_HH

// POSIX
#include <signal.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"

// Recall
#include "recall/stack_crawl.hh"

// relix
#include "relix/api/os_thread_box.hh"
#include "relix/config/syscall_stacks.hh"
#include "relix/signal/sigset_t.hh"
#include "relix/task/task.hh"
#include "relix/task/syscall_stack.hh"
#include "relix/task/vfork_context.hh"


namespace relix
{
	
	class process;
	
	
	class thread : public plus::ref_count< thread >,
	               protected task,
	               public relix::vfork_context
	{
		private:
			typedef recall::stack_frame_pointer frame_ptr;
			
			int its_id;
			int its_saved_errno;
			
			sigset_t its_pending_signals;
			sigset_t its_blocked_signals;

			boost::intrusive_ptr< process > its_process;
			
		#if CONFIG_SYSCALL_STACKS
			
			syscall_stack its_syscall_stack;
			
		#endif
			
			recall::stack_frame_pointer its_stack_frame_ptr;
			
			os_thread_box its_os_thread;
		
		public:
			thread( int id, sigset_t blocked, process& p, bool use_syscall_stack );
			
			~thread();
			
			static thread* from_queue_element( queue_element* that )
			{
				return static_cast< thread* >( that );
			}
			
			task* get_task()  { return this; }
			
			int id() const  { return its_id; }
			
			process& get_process() const;
			
			sigset_t signals_pending() const  { return its_pending_signals; }
			sigset_t signals_blocked() const  { return its_blocked_signals; }
			
			void set_pending_signal( int sig )  { sigaddset( &its_pending_signals, sig ); }
			
			void clear_pending_signal( int sig )  { sigdelset( &its_pending_signals, sig ); }
			
			void block_signals  ( sigset_t sigset );
			void unblock_signals( sigset_t sigset );
			
			void set_signals_blocked( sigset_t sigset )  { its_blocked_signals = sigset; }
			
		#if CONFIG_SYSCALL_STACKS
			
			void* get_syscall_stack_memory() const  { return its_syscall_stack.memory; }
			
		#else
			
			void* get_syscall_stack_memory() const  { return NULL; }
			
		#endif
			
			frame_ptr marked_stack_frame() const  { return its_stack_frame_ptr; }
			
			void mark_current_stack_frame();
			
			void mark_vfork_stack_frame()
			{
				its_stack_frame_ptr = (frame_ptr) get_vfork_frame_pointer();
			}
			
			void clear_stack_frame_mark()  { its_stack_frame_ptr =  NULL; }
			
			bool has_own_os_thread() const  { return its_os_thread.get() != NULL; }
			
			os_thread_id get_os_thread() const;
			
			void swap_os_thread( os_thread_box& other )  { its_os_thread.swap( other ); }
			
			void swap_os_thread( thread& other )  { its_os_thread.swap( other.its_os_thread ); }
			
			void share_os_thread( const thread& other )  { its_os_thread = other.its_os_thread; }
			
			void reset_os_thread()  { its_os_thread.reset(); }
			
			void update_os_thread_param( void* param )
			{
				set_os_thread_param( *its_os_thread.get(), param );
			}
			
			void switch_in();
			void switch_out();
	};
	
#if defined( __RELIX__ )  ||  defined( __APPLE__ )
	
	inline void thread::block_signals  ( sigset_t sigset )  { its_blocked_signals |=  sigset; }
	inline void thread::unblock_signals( sigset_t sigset )  { its_blocked_signals &= ~sigset; }
	
#endif
	
}

#endif
