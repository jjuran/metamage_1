/*
	fd_map.hh
	---------
*/

#ifndef RELIX_TASK_FDMAP_HH
#define RELIX_TASK_FDMAP_HH

// vxo
#include "vxo/ref_count.hh"


namespace vfs
{
	
	struct file_descriptor;
	
}

namespace relix
{
	
	class fd_map;
	
	void destroy( const fd_map* x );
	
}

namespace relix
{
	
	class fd_map : public vxo::ref_count< fd_map >
	{
		private:
			// Private, undefined assignment operator prevents std::swap()
			fd_map& operator=( const fd_map& );
		
		protected:
			// Protected destructor prevents slicing
			~fd_map()  {}
		
		public:
			typedef vfs::file_descriptor file_descriptor;
			
			bool contains( int fd ) const;
			
			file_descriptor& at( int fd );
			
			file_descriptor& operator[]( int fd );
			
			int first_unused( int minimum ) const;
			
			void close( int fd );
			
			void for_each( void (*)( void*, int, const file_descriptor& ), void* = 0L ) const;
			
			void for_each( void (*)( void*, int, file_descriptor& ), void* = 0L );
			
			void clear();
			
			void swap( fd_map& other );
			
			static fd_map* create();
	};
	
	inline void swap( fd_map& a, fd_map& b )
	{
		a.swap( b );
	}
	
	fd_map* duplicate( const fd_map& one );
	
}

#endif
