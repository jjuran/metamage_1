/*
	coprocess.hh
	------------
*/

#ifndef POSEVEN_EXTRAS_COPROCESS_HH
#define POSEVEN_EXTRAS_COPROCESS_HH

// vxo
#include "vxo/ref_count.hh"

// poseven
#ifndef POSEVEN_TYPES_PID_T_HH
#include "poseven/types/pid_t.hh"
#endif
#ifndef POSEVEN_TYPES_WAIT_T_HH
#include "poseven/types/wait_t.hh"
#endif


namespace poseven
{
	
	class coprocess : public vxo::ref_count< coprocess >
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
