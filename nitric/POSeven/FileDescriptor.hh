// =================
// FileDescriptor.hh
// =================

#ifndef POSEVEN_FILEDESCRIPTOR_HH
#define POSEVEN_FILEDESCRIPTOR_HH

// Standard C
#include <errno.h>

// POSIX
#include "fcntl.h"
#include <unistd.h>

// Nucleus
#include "Nucleus/ID.h"
#include "Nucleus/Owned.h"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace POSeven
{
	
	typedef Nucleus::ID< class FileDescriptor_Tag, int >::Type FileDescriptor;
	
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
	
	ssize_t Read( FileDescriptor fd, char* buffer, std::size_t byteCount );
	
	ssize_t Write( FileDescriptor fd, const char* buffer, std::size_t byteCount );
	
	struct POSIX_Io_Details
	{
		typedef std::string file_spec;
		
		typedef FileDescriptor stream;
		
		typedef std::size_t byte_count;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< std::string > : public POSeven::POSIX_Io_Details {};
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_reading( const char* pathname )
	{
		return POSeven::Open( pathname, O_RDONLY );
	}
	
	inline Nucleus::Owned< POSeven::FileDescriptor > open_for_reading( const std::string& pathname )
	{
		return open_for_reading( pathname.c_str() );
	}
	
	template < class ByteCount >
	inline ssize_t read( POSeven::FileDescriptor fd, char* buffer, ByteCount byteCount )
	{
		return POSeven::Read( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
	template < class ByteCount >
	inline ssize_t write( POSeven::FileDescriptor fd, const char* buffer, ByteCount byteCount )
	{
		return POSeven::Write( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
}

#endif

