/*
	thread.hh
	---------
*/

#ifndef RELIX_TASK_THREAD_HH
#define RELIX_TASK_THREAD_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"


namespace relix
{
	
	class process;
	
	
	typedef unsigned long sigset_t;
	
	inline sigset_t sigset_from_signo( int signo )
	{
		return 1 << signo - 1;
	}
	
	class thread : public plus::ref_count< thread >
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
			
			void set_pending_signal( int sig )  { its_pending_signals |= sigset_from_signo( sig ); }
			
			void clear_pending_signal( int sig )  { its_pending_signals &= ~sigset_from_signo( sig ); }
			
			void clear_signals_pending()  { its_pending_signals = 0; }
			
			void block_signals  ( sigset_t sigset )  { its_blocked_signals |=  sigset; }
			void unblock_signals( sigset_t sigset )  { its_blocked_signals &= ~sigset; }
			
			void set_signals_blocked( sigset_t sigset )  { its_blocked_signals = sigset; }
	};
	
}

#endif

