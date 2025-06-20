/*
	private_mmap_swap.hh
	--------------------
*/

#ifndef RELIX_TASK_PRIVATEMMAPSWAP_HH
#define RELIX_TASK_PRIVATEMMAPSWAP_HH

// iota
#include "iota/class.hh"


namespace relix
{
	
	enum allocation_result
	{
		Allocation_failed,
		Allocation_successful,
		Allocation_unnecessary = -1,
	};
	
	class private_mmap_swap
	{
		NON_COPYABLE( private_mmap_swap )
		NO_NEW_DELETE
		
		private:
			typedef char** Handle;
			
			Handle h;
		
		public:
			private_mmap_swap() : h()
			{
			}
			
			~private_mmap_swap();
			
			bool allocated() const  { return h != 0; }  // NULL
			
			allocation_result allocate( unsigned long size );
			
			void reset();
			
			void back_up( const void* canon, unsigned long size ) const;
			void restore(       void* canon, unsigned long size ) const;
	};
	
}

#endif
