/*	=================
 *	FileDescriptor.hh
 *	=================
 */

#ifndef GENIE_FILEDESCRIPTOR_HH
#define GENIE_FILEDESCRIPTOR_HH

// Genie
#include "Genie/IOHandle.hh"
#include "Genie/IORef.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	struct FileDescriptor
	{
		IORef handle;
		bool closeOnExec;
		
		FileDescriptor() : closeOnExec( false )  {}
		
		FileDescriptor( const IORef& handle )
		:
			handle     ( handle ),
			closeOnExec( false  )
		{}
		
		FileDescriptor( const Io::Handle& handle )
		:
			handle     ( NewGenericIO( handle ) ),
			closeOnExec( false  )
		{}
		
		FileDescriptor( const FileDescriptor& other );
		
	};
	
}

#endif

