/*
	mmap_t.hh
	---------
*/

#ifndef POSEVEN_TYPES_MMAP_T_HH
#define POSEVEN_TYPES_MMAP_T_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/mman.h>

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif


namespace poseven
{
	
	struct mmap_t
	{
		void*   addr;
		size_t  len;
	};
	
}

namespace nucleus
{
	
	template <>
	struct null_resource< ::poseven::mmap_t >
	{
		static const ::poseven::mmap_t value;
		
		static const ::poseven::mmap_t& get()  { return value; }
	};
	
	template <>
	struct disposer< ::poseven::mmap_t >
	{
		typedef poseven::mmap_t  argument_type;
		typedef void             result_type;
		
		void operator()( const ::poseven::mmap_t& m ) const
		{
			poseven::handle_destruction_posix_result( ::munmap( m.addr, m.len ) );
		}
	};
	
	template <>
	struct aliveness_traits< ::poseven::mmap_t, disposer< ::poseven::mmap_t > >
	{
		struct aliveness_test
		{
			static bool is_live( const ::poseven::mmap_t& m )
			{
				return m.addr != MAP_FAILED;
			}
		};
	};
	
}

#endif
