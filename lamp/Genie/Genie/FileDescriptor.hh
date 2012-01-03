/*	=================
 *	FileDescriptor.hh
 *	=================
 */

#ifndef GENIE_FILEDESCRIPTOR_HH
#define GENIE_FILEDESCRIPTOR_HH

// Debug
#include "debug/boost_assert.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	struct FileDescriptor
	{
		IOPtr  handle;
		bool   closeOnExec;
		
		FileDescriptor();
		
		FileDescriptor( const IOPtr& handle );
		
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

