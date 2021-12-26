/*
	signal_handlers.hh
	------------------
*/

#ifndef RELIX_TASK_SIGNALHANDLERS_HH
#define RELIX_TASK_SIGNALHANDLERS_HH

// vxo
#include "vxo/ref_count.hh"


// #include <signal.h>
struct sigaction;


namespace relix
{
	
	class signal_handlers;
	
	void destroy( const signal_handlers* x );
	
	
	class signal_handlers : public vxo::ref_count< signal_handlers >
	{
		private:
			// Private, undefined assignment operator prevents std::swap()
			signal_handlers& operator=( const signal_handlers& );
		
		protected:
			// Protected destructor prevents slicing
			~signal_handlers()  {}
		
		public:
			const struct sigaction& get( int signo ) const;
			
			void set( int signo, const struct sigaction& action );
			
			void reset_handlers();
			
			void swap( signal_handlers& other );
			
			static signal_handlers* create();
	};
	
	inline void swap( signal_handlers& a, signal_handlers& b )
	{
		a.swap( b );
	}
	
	signal_handlers* duplicate( const signal_handlers& one );
	
}

#endif
