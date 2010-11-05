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
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
#endif
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif
#ifndef POSEVEN_TYPES_FCNTLCMD_T_HH
#include "poseven/types/fcntl_cmd_t.hh"
#endif
#ifndef POSEVEN_TYPES_FCNTLFDFLAGS_T_HH
#include "poseven/types/fcntl_fd_flags_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif
#ifndef POSEVEN_TYPES_OPENFLAGS_T_HH
#include "poseven/types/open_flags_t.hh"
#endif
#ifndef POSEVEN_TYPES_PID_T_HH
#include "poseven/types/pid_t.hh"
#endif


namespace poseven
{
	
	struct fcntl_returns_void
	{
		typedef void result_type;
		
		static void make_result( int result )
		{
		}
	};
	
	
	template < fcntl_cmd_t cmd >  struct fcntl_traits;
	
	
	template <> struct fcntl_traits< f_dupfd >
	{
		typedef nucleus::owned< fd_t > result_type;
		
		typedef fd_t param_type;
		
		static nucleus::owned< fd_t > make_result( int result )
		{
			return nucleus::owned< fd_t >::seize( fd_t( result ) );
		}
	};
	
#ifdef F_DUPFD_CLOEXEC
	
	template <> struct fcntl_traits< f_dupfd_cloexec > : fcntl_traits< f_dupfd >
	{
	};
	
#endif
	
	template <> struct fcntl_traits< f_getfd >
	{
		typedef fcntl_fd_flags_t result_type;
		
		static fcntl_fd_flags_t make_result( int result )
		{
			return fcntl_fd_flags_t( result );
		}
	};
	
	template <> struct fcntl_traits< f_setfd > : fcntl_returns_void
	{
		typedef fcntl_fd_flags_t param_type;
	};
	
	template <> struct fcntl_traits< f_getfl >
	{
		typedef open_flags_t result_type;
		
		static open_flags_t make_result( int result )
		{
			return open_flags_t( result );
		}
	};
	
	template <> struct fcntl_traits< f_setfl > : fcntl_returns_void
	{
		typedef open_flags_t param_type;
	};
	
	template <> struct fcntl_traits< f_getown >
	{
		typedef pid_t result_type;
		
		static pid_t make_result( int result )
		{
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
		int result = throw_posix_result( ::fcntl( fd, cmd, 0 ) );
		
		return fcntl_traits< cmd >::make_result( result );
	}
	
	template < fcntl_cmd_t cmd >
	inline typename fcntl_traits< cmd >::result_type
	//
	fcntl( fd_t fd, typename fcntl_traits< cmd >::param_type param )
	{
		int result = throw_posix_result( ::fcntl( fd, cmd, (int) param ) );
		
		return fcntl_traits< cmd >::make_result( result );
	}
	
}

#endif

