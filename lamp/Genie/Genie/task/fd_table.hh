/*
	fd_table.hh
	-----------
*/

#ifndef GENIE_TASK_FDTABLE_HH
#define GENIE_TASK_FDTABLE_HH

// plus
#include "plus/ref_count.hh"


namespace vfs
{
	
	struct file_descriptor;
	
}

namespace relix
{
	
	class fd_table;
	
	void destroy( const fd_table* x );
	
}

namespace Genie
{
	
	class fd_table : public plus::ref_count< fd_table >
	{
		private:
			// Private, undefined assignment operator prevents std::swap()
			fd_table& operator=( const fd_table& );
		
		protected:
			// Protected destructor prevents slicing
			~fd_table()  {}
		
		public:
			typedef vfs::file_descriptor file_descriptor;
			
			bool contains( int fd ) const;
			
			file_descriptor& at( int fd );
			
			file_descriptor& operator[]( int fd );
			
			int first_unused( int minimum );
			
			void close( int fd );
			
			void for_each( void (*)( void*, int, const file_descriptor& ), void* = 0L ) const;
			
			void for_each( void (*)( void*, int, file_descriptor& ), void* = 0L );
			
			void clear();
			
			void swap( fd_table& other );
			
			static fd_table* create();
	};
	
	inline void swap( fd_table& a, fd_table& b )
	{
		a.swap( b );
	}
	
	fd_table* duplicate( const fd_table& one );
	
}

#endif

