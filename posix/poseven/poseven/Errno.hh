// Errno.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_ERRNO_HH
#define POSEVEN_ERRNO_HH

// Standard C
#include <errno.h>

// Nucleus
#ifndef NUCLEUS_ERRORCODE_H
#include "Nucleus/ErrorCode.h"
#endif
#ifndef NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nucleus/DestructionExceptionPolicy.h"
#endif
#ifndef NUCLEUS_THEEXCEPTIONBEINGHANDLED_H
#include "Nucleus/TheExceptionBeingHandled.h"
#endif

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	template < int number >
	void register_errno()
	{
		Nucleus::RegisterErrorCode< errno_t, number >();
	}
	
	template < class DestructionExceptionPolicy >
	struct DestructionErrnoPolicy: public DestructionExceptionPolicy
	{
		void HandleDestructionErrno( int error ) const
		{
			try
			{
				throw_errno( error );
				
				DestructionExceptionPolicy::WarnOfDestructionExceptionRisk();
			}
			catch( ... )
			{
				DestructionExceptionPolicy::HandleDestructionException( Nucleus::TheExceptionBeingHandled() );
			}
		}
	};
	
	typedef DestructionErrnoPolicy< Nucleus::DefaultDestructionExceptionPolicy > DefaultDestructionErrnoPolicy;
	
	
	#define DEFINE_ERRNO( c_name, new_name )  DEFINE_ERRORCODE( errno_t, c_name, new_name )
	
	DEFINE_ERRNO( ENOENT, enoent );
	DEFINE_ERRNO( EEXIST, eexist );
	
}

namespace Nucleus
{
	
	template <>
	class ErrorCode< poseven::errno_t, ENOMEM > : public poseven::errno_t,
	                                              public std::bad_alloc,
	                                              public DebuggingContext
	{
		public:
			ErrorCode() : errno_t( ENOMEM )  {}
			
			~ErrorCode() throw ()  {}
	};
	
	template <> struct Converter< poseven::errno_t, std::bad_alloc > : public std::unary_function< std::bad_alloc, poseven::errno_t >
	{
		poseven::errno_t operator()( const std::bad_alloc& ) const
		{
			return poseven::errno_t( ENOMEM );
		}
	};
	
}

#endif

