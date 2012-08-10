/*
	vm_fork.hh
	----------
*/

#ifndef RELIX_TASK_VMFORK_HH
#define RELIX_TASK_VMFORK_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/ref_count.hh"


namespace relix
{
	
	class vm_fork : public plus::ref_count< vm_fork >
	{
		private:
			pid_t its_current_pid;
		
		public:
			vm_fork()
			{
			}
			
			vm_fork( pid_t pid )
			:
				its_current_pid( pid )
			{
			}
			
			long ref_count() const  { return intrusive_ptr_ref_count( this ); }
			
			pid_t getpid() const  { return its_current_pid; }
			
			void setpid( pid_t pid )  { its_current_pid = pid; }
	};
	
}

#endif

