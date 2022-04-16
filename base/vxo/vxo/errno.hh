/*
	errno.hh
	--------
*/

#ifndef VXO_ERRNO_HH
#define VXO_ERRNO_HH

// Standard C
#include <string.h>

// vxo
#include "vxo/error.hh"


namespace vxo
{
	
	enum
	{
		ErrorDomain_none,
		ErrorDomain_errno,
	};
	
	class ErrorCode : public Error
	{
		public:
			static bool test( const Box& box )
			{
				return Error::test( box )  &&  box.ternary_byte() > 0;
			}
			
			static bool test_in_context( const Box& box, uint8_t error_domain )
			{
				return test( box )  &&  box.ternary_byte() == error_domain;
			}
			
			ErrorCode( const char* s, uint8_t error_domain, int32_t errnum )
			:
				Error( s )
			{
				set_ternary_byte( error_domain );
				
				u.str.capacity = errnum;
			}
			
			int32_t get() const  { return u.str.capacity; }
	};
	
	class Errno : public ErrorCode
	{
		public:
			static bool test( const Box& box )
			{
				return ErrorCode::test_in_context( box, ErrorDomain_errno );
			}
			
			Errno( int errnum, const char* message )
			:
				ErrorCode( message, ErrorDomain_errno, errnum )
			{
			}
			
			explicit Errno( int errnum )
			:
				ErrorCode( strerror( errnum ), ErrorDomain_errno, errnum )
			{
			}
	};
	
}

#endif
