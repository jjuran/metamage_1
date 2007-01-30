// ========
// Errno.hh
// ========

#ifndef POSEVEN_ERRNO_HH
#define POSEVEN_ERRNO_HH

// POSIX
#include "errno.h"

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


#ifdef NUCLEUS_DEBUG
#define ThrowPOSIXResult(result)  ThrowPOSIXResultInternalDebug( result, #result, __FILE__, __LINE__ )
#else
#define ThrowPOSIXResult(result)  ThrowPOSIXResultInternal( result )
#endif

#ifdef NUCLEUS_DEBUG
#define	ThrowErrno(error)  ThrowErrnoInternalDebug(error, "", __FILE__, __LINE__)
#else
#define	ThrowErrno(error)	ThrowErrnoInternal(error)
#endif

#define ThrowNegativeOne(result)  if ( (result) == -1 )  { ThrowErrno( errno ); } else


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
	
	typedef void (*ErrnoLoggingProc) ( int, const char *, const char *, int );
	ErrnoLoggingProc SetErrnoLoggingProc( ErrnoLoggingProc newProc );
	
	void ThrowPOSIXResultInternalDebug( POSIXResult error, const char* text, const char * file, int line );
	
	void ThrowErrnoInternalDebug( Errno number, const char* text, const char * file, int line );
	
	void ThrowPOSIXResultInternal( POSIXResult error );
	
	void ThrowErrnoInternal( Errno number );
	
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

namespace Nucleus
{
	
	template <>
	class ErrorCode< POSeven::Errno, ENOMEM > : public POSeven::Errno, std::bad_alloc
	{
		public:
			ErrorCode() : Errno( ENOMEM )  {}
	};
	
	template <>
	class ErrorCode< POSeven::Errno, EAGAIN > : public POSeven::Errno, io::no_input_pending
	{
		public:
			ErrorCode() : Errno( EAGAIN )  {}
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

