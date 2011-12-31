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
		
		FileDescriptor() : closeOnExec( false )  {}
		
		FileDescriptor( const IOPtr& handle ) : handle     ( handle ),
		                                        closeOnExec( false  )
		{
		}
		
		void swap( FileDescriptor& other );
	};
	
	inline void swap( FileDescriptor& a, FileDescriptor& b )
	{
		a.swap( b );
	}
	
}

#endif

