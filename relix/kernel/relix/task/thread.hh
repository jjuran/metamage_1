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

// relix
#include "relix/signal/sigset_t.hh"
#include "relix/task/vfork_context.hh"


namespace relix
{
	
	class process;
	
	
	class thread : public plus::ref_count< thread >,
	               public relix::vfork_context
	{
		private:
			int its_id;
			
			sigset_t its_pending_signals;
			sigset_t its_blocked_signals;

			boost::intrusive_ptr< process > its_process;
		
		public:
			thread( int id, sigset_t blocked, process& p );
			
			virtual ~thread();
			
			int id() const  { return its_id; }
			
			process& get_process() const;
			
			sigset_t signals_pending() const  { return its_pending_signals; }
			sigset_t signals_blocked() const  { return its_blocked_signals; }
			
			void set_pending_signal( int sig )  { sigaddset( &its_pending_signals, sig ); }
			
			void clear_pending_signal( int sig )  { sigdelset( &its_pending_signals, sig ); }
			
			void clear_signals_pending()  { sigemptyset( &its_pending_signals ); }
			
			void block_signals  ( sigset_t sigset );
			void unblock_signals( sigset_t sigset );
			
			void set_signals_blocked( sigset_t sigset )  { its_blocked_signals = sigset; }
	};
	
#if defined( __RELIX__ )  ||  defined( __APPLE__ )
	
	inline void thread::block_signals  ( sigset_t sigset )  { its_blocked_signals |=  sigset; }
	inline void thread::unblock_signals( sigset_t sigset )  { its_blocked_signals &= ~sigset; }
	
#endif
	
}

#endif

