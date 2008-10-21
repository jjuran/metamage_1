// Open.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_OPEN_HH
#define POSEVEN_OPEN_HH

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/functions/close.hh"
#include "POSeven/types/mode_t.hh"


namespace poseven
{
	
	enum oflag_t
	{
		o_rdonly = O_RDONLY,
		o_wronly = O_WRONLY,
		o_rdwr   = O_RDWR,
		o_creat  = O_CREAT,
		o_trunc  = O_TRUNC,
		o_append = O_APPEND,
		o_excl   = O_EXCL,
		
	#ifdef O_TRUNC_LAZY
		
		o_trunc_lazy = O_TRUNC_LAZY,
		
	#endif
		
		oflag_t_max = Nucleus::Enumeration_Traits< int >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( oflag_t )
	
	Nucleus::Owned< fd_t > open( const char* name, oflag_t oflag, mode_t mode = mode_t( 644 ) );
	
	inline Nucleus::Owned< fd_t > open( const std::string& name, oflag_t oflag, mode_t mode = mode_t( 644 ) )
	{
		return open( name.c_str(), oflag, mode );
	}
	
}

namespace io
{
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdonly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_reading( const std::string& pathname, overload = overload() )
	{
		return open_for_reading( pathname.c_str(), overload() );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_wronly );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_writing( const std::string& pathname, overload = overload() )
	{
		return open_for_writing( pathname.c_str(), overload() );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_io( const char* pathname, overload = overload() )
	{
		return poseven::open( pathname, poseven::o_rdwr );
	}
	
	inline Nucleus::Owned< poseven::fd_t > open_for_io( const std::string& pathname, overload = overload() )
	{
		return open_for_io( pathname.c_str(), overload() );
	}
	
}

#endif

