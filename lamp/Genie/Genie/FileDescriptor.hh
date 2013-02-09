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
		bool   closeOnExec;
		
		FileDescriptor();
		
		FileDescriptor( const filehandle_ptr& handle );
		
		FileDescriptor( const FileDescriptor& other );
		
		~FileDescriptor();
		
		FileDescriptor& operator=( const FileDescriptor& );
		
		void swap( FileDescriptor& other );
	};
	
	inline void swap( FileDescriptor& a, FileDescriptor& b )
	{
		a.swap( b );
	}
	
}

#endif

