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

#ifndef NUCLEUS_ERRORCODE_H
#include "Nucleus/ErrorCode.h"
#endif
#ifndef NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nucleus/DestructionExceptionPolicy.h"
#endif
#ifndef NUCLEUS_THEEXCEPTIONBEINGHANDLED_H
#include "Nucleus/TheExceptionBeingHandled.h"
#endif

#ifndef IO_IO_HH
#include "io/io.hh"
#endif


#define	ThrowErrno(       error  )  ThrowErrno_Inline      ( error,  NUCLEUS_CREATE_DEBUGGING_CONTEXT() )
#define	ThrowPOSIXResult( result )  ThrowPOSIXResult_Inline( result, NUCLEUS_CREATE_DEBUGGING_CONTEXT() )


namespace POSeven
{
	
	class POSIXResult
	{
		private:
			int result;
		
		public:
			POSIXResult(       ) : result( 0 )  {}
			POSIXResult( int x ) : result( x )  {}
			
			static POSIXResult Make( int x )    { return POSIXResult( x ); }
			
			int Get() const               { return result; }
			operator int() const          { return result; }
	};
	
	class Errno
	{
		private:
			int itsNumber;
		
		public:
			typedef int ErrorNumber;
			
			Errno(       ) : itsNumber( 0 )  {}
			Errno( int n ) : itsNumber( n )  {}
			
			static Errno Make( int n )    { return Errno( n ); }
			
			int Get() const               { return itsNumber; }
			operator int() const          { return itsNumber; }
	};
	
	template < int number >
	void RegisterErrno()
	{
		Nucleus::RegisterErrorCode< Errno, number >();
	}
	
	void ThrowErrno_Internal( Errno number, const Nucleus::DebuggingContext& debugging );
	
	inline void ThrowErrno_Inline( int error, const Nucleus::DebuggingContext& debugging )
	{
		if ( error != 0 )
		{
			ThrowErrno_Internal( error, debugging );
		}
	}
	
	inline int ThrowPOSIXResult_Inline( int result, const Nucleus::DebuggingContext& debugging )
	{
		if ( result < 0 )
		{
			ThrowErrno_Internal( errno, debugging );
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
				ThrowErrno( error );
				DestructionExceptionPolicy::WarnOfDestructionExceptionRisk();
			}
			catch( ... )
			{
				DestructionExceptionPolicy::HandleDestructionException( Nucleus::TheExceptionBeingHandled() );
			}
		}
	};
	
	typedef DestructionErrnoPolicy< Nucleus::DefaultDestructionExceptionPolicy > DefaultDestructionErrnoPolicy;
	
	typedef Nucleus::ErrorCode< Errno, ENOENT > ENoEnt;
	typedef Nucleus::ErrorCode< Errno, EEXIST > EExist;
	
}

namespace poseven
{
	
	typedef POSeven::Errno errno_t;
	
}

namespace Nucleus
{
	
	template <>
	class ErrorCode< POSeven::Errno, ENOMEM > : public POSeven::Errno,
	                                            public std::bad_alloc,
	                                            public DebuggingContext
	{
		public:
			ErrorCode() : Errno( ENOMEM )  {}
			ErrorCode( const DebuggingContext& debugging ) : Errno( ENOMEM ),
			                                                 DebuggingContext( debugging )
			{
			}
	};
	
	template <>
	class ErrorCode< POSeven::Errno, EAGAIN > : public POSeven::Errno,
	                                            public io::no_input_pending,
	                                            public DebuggingContext
	{
		public:
			ErrorCode() : Errno( EAGAIN )  {}
			ErrorCode( const DebuggingContext& debugging ) : Errno( EAGAIN ),
			                                                 DebuggingContext( debugging )
			{
			}
	};
	
	template <> struct Converter< POSeven::Errno, std::bad_alloc >: public std::unary_function< std::bad_alloc, POSeven::Errno >
	{
		POSeven::Errno operator()( const std::bad_alloc& ) const
		{
			return POSeven::Errno( ENOMEM );
		}
	};
	
	template <> struct Converter< POSeven::Errno, io::no_input_pending >: public std::unary_function< io::no_input_pending, POSeven::Errno >
	{
		POSeven::Errno operator()( const io::no_input_pending& ) const
		{
			return POSeven::Errno( EAGAIN );
		}
	};
	
}

#endif

