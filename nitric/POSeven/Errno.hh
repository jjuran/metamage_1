// Errno.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
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

// Io
#ifndef IO_IO_HH
#include "io/io.hh"
#endif


namespace poseven
{
	
	class errno_t
	{
		private:
			int itsNumber;
		
		public:
			typedef int ErrorNumber;
			
			errno_t(       ) : itsNumber( 0 )  {}
			errno_t( int n ) : itsNumber( n )  {}
			
			static errno_t Make( int n )    { return errno_t( n ); }
			
			int Get() const               { return itsNumber; }
			operator int() const          { return itsNumber; }
	};
	
	template < int number >
	void register_errno()
	{
		Nucleus::RegisterErrorCode< errno_t, number >();
	}
	
	void throw_errno_internal( errno_t number );
	
	inline void throw_errno( int error )
	{
		if ( error != 0 )
		{
			throw_errno_internal( error );
		}
	}
	
	inline int throw_posix_result( int result )
	{
		if ( result < 0 )
		{
			throw_errno_internal( errno );
		}
		
		return result;
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
	
	typedef Nucleus::ErrorCode< errno_t, ENOENT > enoent;
	typedef Nucleus::ErrorCode< errno_t, EEXIST > eexist;
	
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
			ErrorCode( const DebuggingContext& debugging ) : errno_t( ENOMEM ),
			                                                 DebuggingContext( debugging )
			{
			}
			
			~ErrorCode() throw ()  {}
	};
	
	template <>
	class ErrorCode< poseven::errno_t, EAGAIN > : public poseven::errno_t,
	                                              public io::no_input_pending,
	                                              public DebuggingContext
	{
		public:
			ErrorCode() : errno_t( EAGAIN )  {}
			ErrorCode( const DebuggingContext& debugging ) : errno_t( EAGAIN ),
			                                                 DebuggingContext( debugging )
			{
			}
	};
	
	template <> struct Converter< poseven::errno_t, std::bad_alloc > : public std::unary_function< std::bad_alloc, poseven::errno_t >
	{
		poseven::errno_t operator()( const std::bad_alloc& ) const
		{
			return poseven::errno_t( ENOMEM );
		}
	};
	
	template <> struct Converter< poseven::errno_t, io::no_input_pending > : public std::unary_function< io::no_input_pending, poseven::errno_t >
	{
		poseven::errno_t operator()( const io::no_input_pending& ) const
		{
			return poseven::errno_t( EAGAIN );
		}
	};
	
}

#endif

