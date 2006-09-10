// =================
// FileDescriptor.hh
// =================

#ifndef POSEVEN_FILEDESCRIPTOR_HH
#define POSEVEN_FILEDESCRIPTOR_HH

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>

// Nucleus
#include "Nucleus/ID.h"
#include "Nucleus/Owned.h"

// POSeven
#include "POSeven/Errno.hh"


namespace POSeven
{
	
	namespace NN = Nucleus;
	
	typedef NN::ID< struct FileDescriptor_Tag, int >::Type FileDescriptor;
	
	namespace Constants
	{
		
		const static FileDescriptor kStdIn_FileNo  = FileDescriptor( STDIN_FILENO  );
		const static FileDescriptor kStdOut_FileNo = FileDescriptor( STDOUT_FILENO );
		const static FileDescriptor kStdErr_FileNo = FileDescriptor( STDERR_FILENO );
		
	}
	
	using namespace Constants;
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< POSeven::FileDescriptor > : std::unary_function< POSeven::FileDescriptor, void >,
	                                             POSeven::DefaultDestructionErrnoPolicy
	{
		void operator()( POSeven::FileDescriptor fd ) const
		{
			// FIXME
			// HandleDestructionPOSIXError( ::close( fd ) );
			
			if ( ::close( fd ) == -1 )
			{
				HandleDestructionErrno( errno );
			}
		}
	};
	
}

namespace POSeven
{
	
	Nucleus::Owned< FileDescriptor > Open( const char* name, int oflag, int mode = 0 );
	
	void Close( Nucleus::Owned< FileDescriptor > fd );
	
	int Read( FileDescriptor fd );
	int Write( FileDescriptor fd );
	
}

#endif

