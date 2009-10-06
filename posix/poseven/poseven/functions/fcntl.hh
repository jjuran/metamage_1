// fcntl.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FCNTL_HH
#define POSEVEN_FUNCTIONS_FCNTL_HH

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/errno_t.hh"
#include "poseven/types/fcntl_cmd_t.hh"
#include "poseven/types/fcntl_fd_flags_t.hh"
#include "poseven/types/fd_t.hh"
#include "poseven/types/open_flags_t.hh"
#include "poseven/types/pid_t.hh"


namespace poseven
{
	
	inline void throw_errno_on_negative( int result )
	{
		if ( result < 0 )
		{
			throw_errno( errno );
		}
	}
	
	inline void throw_errno_on_negative_one( int result )
	{
		if ( result == -1 )
		{
			throw_errno( errno );
		}
	}
	
	struct throws_errno_on_negative
	{
		static void check_result( int result )
		{
			throw_errno_on_negative( result );
		}
	};
	
	struct throws_errno_on_negative_one
	{
		static void check_result( int result )
		{
			throw_errno_on_negative_one( result );
		}
	};
	
	struct fcntl_returns_void : throws_errno_on_negative_one
	{
		typedef void result_type;
		
		static void make_result( int result )
		{
			check_result( result );
		}
	};
	
	
	template < fcntl_cmd_t cmd >  struct fcntl_traits;
	
	
	template <> struct fcntl_traits< f_dupfd > : throws_errno_on_negative
	{
		typedef Nucleus::Owned< fd_t > result_type;
		
		typedef fd_t param_type;
		
		static Nucleus::Owned< fd_t > make_result( int result )
		{
			check_result( result );
			
			return Nucleus::Owned< fd_t >::Seize( fd_t( result ) );
		}
	};
	
#ifdef F_DUPFD_CLOEXEC
	
	template <> struct fcntl_traits< f_dupfd_cloexec > : fcntl_traits< f_dupfd >
	{
	};
	
#endif
	
	template <> struct fcntl_traits< f_getfd > : throws_errno_on_negative
	{
		typedef fcntl_fd_flags_t result_type;
		
		static fcntl_fd_flags_t make_result( int result )
		{
			check_result( result );
			
			return fcntl_fd_flags_t( result );
		}
	};
	
	template <> struct fcntl_traits< f_setfd > : fcntl_returns_void
	{
		typedef fcntl_fd_flags_t param_type;
	};
	
	template <> struct fcntl_traits< f_getfl > : throws_errno_on_negative
	{
		typedef open_flags_t result_type;
		
		static open_flags_t make_result( int result )
		{
			check_result( result );
			
			return open_flags_t( result );
		}
	};
	
	template <> struct fcntl_traits< f_setfl > : fcntl_returns_void
	{
		typedef open_flags_t param_type;
	};
	
	template <> struct fcntl_traits< f_getown > : throws_errno_on_negative_one
	{
		typedef pid_t result_type;
		
		static pid_t make_result( int result )
		{
			check_result( result );
			
			return pid_t( result );
		}
	};
	
	template <> struct fcntl_traits< f_setown > : fcntl_returns_void
	{
		typedef pid_t param_type;
	};
	
	template <> struct fcntl_traits< f_getlk > : fcntl_returns_void
	{
		typedef struct flock* param_type;
	};
	
	template <> struct fcntl_traits< f_setlk > : fcntl_returns_void
	{
		typedef struct flock* param_type;
	};
	
	template <> struct fcntl_traits< f_setlkw > : fcntl_returns_void
	{
		typedef struct flock* param_type;
	};
	
	
	template < fcntl_cmd_t cmd >
	inline typename fcntl_traits< cmd >::result_type
	//
	fcntl( fd_t fd )
	{
		return fcntl_traits< cmd >::make_result( ::fcntl( fd, cmd, 0 ) );
	}
	
	template < fcntl_cmd_t cmd >
	inline typename fcntl_traits< cmd >::result_type
	//
	fcntl( fd_t fd, typename fcntl_traits< cmd >::param_type param )
	{
		return fcntl_traits< cmd >::make_result( ::fcntl( fd, cmd, (int) param ) );
	}
	
}

#endif

