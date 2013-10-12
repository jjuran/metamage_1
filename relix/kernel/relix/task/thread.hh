/*
	thread.hh
	---------
*/

#ifndef RELIX_TASK_THREAD_HH
#define RELIX_TASK_THREAD_HH

// plus
#include "plus/ref_count.hh"


namespace relix
{
	
	class process;
	
	
	class thread : public plus::ref_count< thread >
	{
		private:
			int its_id;
		
		public:
			thread( int id );
			
			virtual ~thread();
			
			int id() const  { return its_id; }
	};
	
}

#endif

