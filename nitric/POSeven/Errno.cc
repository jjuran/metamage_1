// Errno.cc
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "POSeven/Errno.hh"


namespace poseven
{
	
	static void register_posix_errnos();
	
	class posix_errnos_registration
	{
		public:
			posix_errnos_registration()  { register_posix_errnos(); }
	};
	
	static posix_errnos_registration the_registration;
	
	
	void throw_errno_internal( errno_t number )
	{
		Nucleus::ThrowErrorCode< errno_t >( number );
	}
	
	void register_posix_errnos()
	{
		register_errno< ENOENT >();
		register_errno< ENOMEM >();
		register_errno< EAGAIN >();
	}
	
}

