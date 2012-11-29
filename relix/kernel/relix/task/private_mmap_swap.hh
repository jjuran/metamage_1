/*
	private_mmap_swap.hh
	--------------------
*/

#ifndef RELIX_TASK_PRIVATEMMAPSWAP_HH
#define RELIX_TASK_PRIVATEMMAPSWAP_HH


namespace relix
{
	
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
			
			void allocate( unsigned long size );
			
			void reset();
			
			void back_up( const void* canon, unsigned long size ) const;
			void restore(       void* canon, unsigned long size ) const;
	};
	
}

#endif
