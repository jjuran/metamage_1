/*
	error.hh
	--------
*/

#ifndef VXO_ERROR_HH
#define VXO_ERROR_HH

// vxo
#include "vxo/string.hh"


namespace vxo
{
	
	class Error : public StaticString
	{
		public:
			static bool test( const Box& box )
			{
				return box.control_byte() < 0  &&  box.subtype_byte() == -1;
			}
			
			explicit Error( const char* s ) : StaticString( s )
			{
				set_subtype_byte( Box_error );
			}
			
			const char* get() const  { return u.str.pointer; }
	};
	
	template < class Value >
	class Expected : public Box
	{
		public:
			Expected( const Error& error ) : Box( error )  {}
			Expected( const Value& value ) : Box( value )  {}
			
			const char* error() const
			{
				return is< Error >() ? error_cast().get() : 0;  // NULL
			}
			
			Error&       error_cast()        { return *(Error*)       this; }
			Error const& error_cast() const  { return *(Error const*) this; }
			
			Value&       value_cast()        { return *(Value*)       this; }
			Value const& value_cast() const  { return *(Value const*) this; }
	};
	
	extern const char out_of_memory[];
	
}

#endif
