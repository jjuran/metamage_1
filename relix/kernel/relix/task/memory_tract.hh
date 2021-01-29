/*
	memory_tract.hh
	---------------
*/

#ifndef RELIX_TASK_MEMORYTRACT_HH
#define RELIX_TASK_MEMORYTRACT_HH

// debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vfs
#include "vfs/memory_mapping.hh"

// Relix
#include "relix/task/private_mmap_swap.hh"


namespace relix
{
	
	class memory_tract
	{
		private:
			boost::intrusive_ptr< const vfs::memory_mapping > its_mapping;
			
			private_mmap_swap its_swap;
			
			// non-copyable
			memory_tract& operator=( const memory_tract& );
		
		public:
			memory_tract()
			{
			}
			
			memory_tract( const memory_tract& other )
			:
				its_mapping( other.its_mapping ),
				its_swap()
			{
			}
			
			memory_tract( const vfs::memory_mapping* mapping )
			:
				its_mapping( mapping ),
				its_swap()
			{
			}
			
			const vfs::memory_mapping* get() const  { return its_mapping.get(); }
			
			void* get_address() const  { return its_mapping->get_address(); }
			
			allocation_result allocate();
			
			void back_up();
			void restore();
	};
	
}

#endif
