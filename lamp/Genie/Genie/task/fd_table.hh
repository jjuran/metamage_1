/*
	fd_table.hh
	-----------
*/

#ifndef GENIE_TASK_FDTABLE_HH
#define GENIE_TASK_FDTABLE_HH

// Standard C++
#include <vector>

// Genie
#include "Genie/FileDescriptor.hh"


namespace Genie
{
	
	class fd_table
	{
		private:
			std::vector< FileDescriptor > its_fds;
		
		public:
			bool contains( int fd ) const;
			
			FileDescriptor& at( int fd );
			
			FileDescriptor& operator[]( int fd );
			
			int first_unused( int minimum );
			
			void close( int fd );
			
			void for_each( void (*)( void*, int, const FileDescriptor& ), void* = NULL ) const;
			
			void for_each( void (*)( void*, int, FileDescriptor& ), void* = NULL );
			
			void clear();
	};
	
}

#endif

