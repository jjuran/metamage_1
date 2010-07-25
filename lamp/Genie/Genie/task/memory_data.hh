/*
	memory_data.hh
	--------------
*/

#ifndef GENIE_TASK_MEMORYDATA_HH
#define GENIE_TASK_MEMORYDATA_HH

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"


namespace Genie
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
			
			char** get_argv();
			char** get_envp();
	};
	
	inline void swap( memory_data& a, memory_data& b )
	{
		a.swap( b );
	}
	
	memory_data* duplicate( const memory_data& one );
}

#endif

