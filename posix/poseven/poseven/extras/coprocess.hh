/*
	coprocess.hh
	------------
*/

#ifndef POSEVEN_EXTRAS_COPROCESS_HH
#define POSEVEN_EXTRAS_COPROCESS_HH

// plus
#include "plus/ref_count.hh"

// poseven
#include "poseven/types/pid_t.hh"
#include "poseven/types/wait_t.hh"


namespace poseven
{
	
	class coprocess : public plus::ref_count< coprocess >
	{
		private:
			pid_t its_pid;
		
		private:
			// non-copyable
			coprocess           ( const coprocess& );
			coprocess& operator=( const coprocess& );
		
		public:
			coprocess( pid_t pid ) : its_pid( pid )
			{
			}
			
			explicit coprocess( volatile coprocess& m ) : its_pid( m.its_pid )
			{
				m.its_pid = pid_t();
			}
			
			~coprocess()
			{
				if ( its_pid != 0 )
				{
					terminate();
				}
			}
			
			volatile coprocess& move()
			{
				return *this;
			}
			
			void kill( int signum ) const;
			
			wait_t wait();
			
			wait_t terminate();
	};
	
}

#endif
