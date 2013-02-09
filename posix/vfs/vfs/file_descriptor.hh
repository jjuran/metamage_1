/*
	file_descriptor.hh
	------------------
*/

#ifndef VFS_FILEDESCRIPTOR_HH
#define VFS_FILEDESCRIPTOR_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	struct file_descriptor
	{
		typedef vfs::filehandle_ptr filehandle_ptr;
		
		filehandle_ptr  handle;
		bool            close_on_exec;
		
		file_descriptor();
		
		file_descriptor( const filehandle_ptr& handle );
		
		file_descriptor( const file_descriptor& other );
		
		~file_descriptor();
		
		file_descriptor& operator=( const file_descriptor& );
		
		void swap( file_descriptor& other );
		
		bool will_close_on_exec() const
		{
			return close_on_exec;
		}
		
		void set_to_close_on_exec( bool will_close )
		{
			close_on_exec = will_close;
		}
	};
	
	inline void swap( file_descriptor& a, file_descriptor& b )
	{
		a.swap( b );
	}
	
}

#endif

