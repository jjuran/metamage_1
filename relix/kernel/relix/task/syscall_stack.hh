/*
	syscall_stack.hh
	----------------
*/

#ifndef RELIX_TASK_SYSCALLSTACK_HH
#define RELIX_TASK_SYSCALLSTACK_HH


namespace relix
{
	
	class syscall_stack
	{
		public:
			void* memory;
		
		private:
			// non-copyable
			syscall_stack           ( const syscall_stack& );
			syscall_stack& operator=( const syscall_stack& );
		
		public:
			void allocate();
			
			syscall_stack( bool allocating = false ) : memory()
			{
				if ( allocating )
				{
					allocate();
				}
			}
			
			~syscall_stack();
	};
	
}

#endif
