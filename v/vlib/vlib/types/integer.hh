/*
	integer.hh
	----------
*/

#ifndef VLIB_TYPES_INTEGER_HH
#define VLIB_TYPES_INTEGER_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct type_info;
	
	class Integer : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_number;
			}
			
			static Value coerce( const Value& v );
			
			Integer() : Value( 0 ) {}
			
			Integer( unsigned i ) : Value( i ) {}
			Integer(      int i ) : Value( i ) {}
			
			Integer( unsigned long i ) : Value( i ) {}
			Integer(          long i ) : Value( i ) {}
			
			Integer( unsigned long long i ) : Value( i ) {}
			Integer(          long long i ) : Value( i ) {}
	};
	
	extern const type_info integer_vtype;
	
}

#endif
