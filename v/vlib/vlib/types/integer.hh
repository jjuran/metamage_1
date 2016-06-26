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
		typedef plus::integer integer;
		
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_number;
			}
			
			static Value coerce( const Value& v );
			
		#define IBOX( i )  plus::ibox( i ).move()
			
			Integer() : Value( IBOX( 0ul ), V_int ) {}
			
			Integer( unsigned i ) : Value( IBOX( (unsigned long) i ), V_int ) {}
			Integer(      int i ) : Value( IBOX( (long)          i ), V_int ) {}
			
			Integer( unsigned long i ) : Value( IBOX( i ), V_int ) {}
			Integer(          long i ) : Value( IBOX( i ), V_int ) {}
			
			Integer( unsigned long long i ) : Value( IBOX( i ), V_int ) {}
			Integer(          long long i ) : Value( IBOX( i ), V_int ) {}
			
		#undef IBOX
			
			Integer( const integer& i ) : Value( (const vu_ibox&) i, V_int ) {}
	};
	
	extern const type_info integer_vtype;
	
}

#endif
