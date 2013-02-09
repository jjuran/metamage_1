/*	=================
 *	FileDescriptor.hh
 *	=================
 */

#ifndef GENIE_FILEDESCRIPTOR_HH
#define GENIE_FILEDESCRIPTOR_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace Genie
{
	
	struct FileDescriptor
	{
		typedef vfs::filehandle_ptr filehandle_ptr;
		
		filehandle_ptr  handle;
		bool            close_on_exec;
		
		FileDescriptor();
		
		FileDescriptor( const filehandle_ptr& handle );
		
		FileDescriptor( const FileDescriptor& other );
		
		~FileDescriptor();
		
		FileDescriptor& operator=( const FileDescriptor& );
		
		void swap( FileDescriptor& other );
		
		bool will_close_on_exec() const
		{
			return close_on_exec;
		}
		
		void set_to_close_on_exec( bool will_close )
		{
			close_on_exec = will_close;
		}
	};
	
	inline void swap( FileDescriptor& a, FileDescriptor& b )
	{
		a.swap( b );
	}
	
}

#endif

