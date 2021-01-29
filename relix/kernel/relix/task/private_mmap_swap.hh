/*
	private_mmap_swap.hh
	--------------------
*/

#ifndef RELIX_TASK_PRIVATEMMAPSWAP_HH
#define RELIX_TASK_PRIVATEMMAPSWAP_HH


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
		private:
			typedef char** Handle;
			
			Handle h;
			
			// non-copyable
			private_mmap_swap           ( const private_mmap_swap& );
			private_mmap_swap& operator=( const private_mmap_swap& );
		
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
