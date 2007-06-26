// FileDescriptor.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FILEDESCRIPTOR_HH
#define POSEVEN_FILEDESCRIPTOR_HH

// Standard C
#include <errno.h>

// POSIX
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

struct stat;

namespace POSeven
{
	
	void Close( Nucleus::Owned< FileDescriptor > fd );
	
	ssize_t Read( FileDescriptor fd, char* buffer, std::size_t byteCount );
	
	ssize_t Write( FileDescriptor fd, const char* buffer, std::size_t byteCount );
	
	struct POSIX_Io_Details
	{
		typedef std::string file_spec;
		typedef std::string optimized_directory_spec;
		
		typedef const std::string& filename_parameter;
		typedef       std::string  filename_result;
		
		typedef struct ::stat file_catalog_record;
		
		typedef FileDescriptor stream;
		
		typedef std::size_t byte_count;
		
		typedef off_t position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< std::string > : public POSeven::POSIX_Io_Details {};
	
	template < class ByteCount >
	inline ssize_t read( POSeven::FileDescriptor fd, char* buffer, ByteCount byteCount, overload = overload() )
	{
		return POSeven::Read( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
	template < class ByteCount >
	inline ssize_t write( POSeven::FileDescriptor fd, const char* buffer, ByteCount byteCount, overload )
	{
		return POSeven::Write( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
}

#endif

