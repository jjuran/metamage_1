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

// Nucleus
#include "Nucleus/Owned.h"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/functions/write.hh"
#include "POSeven/types/fd_t.hh"


namespace Nucleus
{
	
	template <>
	struct Disposer< poseven::fd_t > : std::unary_function< poseven::fd_t, void >,
	                                   poseven::DefaultDestructionErrnoPolicy
	{
		void operator()( poseven::fd_t fd ) const
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

namespace poseven
{
	
	inline void close( Nucleus::Owned< fd_t > fd )
	{
		throw_posix_result( ::close( fd.Release() ) );
	}
	
	inline ssize_t read( fd_t fd, char* buffer, std::size_t bytes_requested )
	{
		return throw_posix_result( ::read( fd, buffer, bytes_requested ) );
	}
	
}

struct stat;

namespace poseven
{
	
	struct posix_io_details
	{
		typedef std::string file_spec;
		typedef std::string optimized_directory_spec;
		
		typedef const std::string& filename_parameter;
		typedef       std::string  filename_result;
		
		typedef struct ::stat file_catalog_record;
		
		typedef fd_t stream;
		
		typedef std::size_t byte_count;
		
		typedef off_t position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< std::string   > : public poseven::posix_io_details {};
	template <> struct iostream_traits< poseven::fd_t > : public poseven::posix_io_details {};
	
	template < class ByteCount >
	inline ssize_t read( poseven::fd_t fd, char* buffer, ByteCount byteCount, overload = overload() )
	{
		return poseven::read( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
	template < class ByteCount >
	inline ssize_t write( poseven::fd_t fd, const char* buffer, ByteCount byteCount, overload = overload() )
	{
		return poseven::write( fd, buffer, Nucleus::Convert< std::size_t >( byteCount ) );
	}
	
}

#endif

