/*
	memory_data.hh
	--------------
*/

#ifndef RELIX_TASK_MEMORYDATA_HH
#define RELIX_TASK_MEMORYDATA_HH

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// vfs
#include "vfs/memory_mapping_fwd.hh"


namespace relix
{
	
	class memory_data;
	
	void destroy( const memory_data* x );
	
	class memory_data : public plus::ref_count< memory_data >
	{
		private:
			// Non-copyable (prevents slicing, including std::swap())
			memory_data           ( const memory_data& );
			memory_data& operator=( const memory_data& );
		
		protected:
			// Protected constructor prevents dynamic allocation
			memory_data()  {}
			
			// Protected destructor prevents slicing
			~memory_data()  {}
		
		public:
			void swap( memory_data& other );
			
			static memory_data* create();
			
			const plus::string& get_cmdline() const;
			
			void set_argv( const char *const *argv );
			void set_envp( const char *const *envp );
			
			int get_argc() const;
			
			char* const* get_argv();
			char* const* get_envp();
			
			typedef void* addr_t;
			typedef unsigned long size_t;  // FIXME for 64-bit Windows
			
			addr_t add_memory_mapping( const vfs::memory_mapping* mapping );
			
			void msync_memory_mapping( addr_t addr, size_t len, int flags );
			
			void remove_memory_mapping( addr_t key );
			
			void clear_memory_mappings();
			
			void back_up();
			void restore();
	};
	
	inline void swap( memory_data& a, memory_data& b )
	{
		a.swap( b );
	}
	
	memory_data* duplicate( const memory_data& one );
	
}

#endif
