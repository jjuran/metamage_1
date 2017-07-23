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
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch integer_dispatch;
	
	class Integer : public Value
	{
		typedef bignum::integer integer;
		
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_number;
			}
			
			static Value coerce( const Value& v );
			
		#define V_INT  V_int, &integer_dispatch
		#define IBOX( i )  bignum::ibox( i ).move()
			
			Integer() : Value( IBOX( 0ul ), V_INT ) {}
			
			Integer( unsigned i ) : Value( IBOX( (unsigned long) i ), V_INT ) {}
			Integer(      int i ) : Value( IBOX( (long)          i ), V_INT ) {}
			
			Integer( unsigned long i ) : Value( IBOX( i ), V_INT ) {}
			Integer(          long i ) : Value( IBOX( i ), V_INT ) {}
			
			Integer( unsigned long long i ) : Value( IBOX( i ), V_INT ) {}
			Integer(          long long i ) : Value( IBOX( i ), V_INT ) {}
			
		#undef IBOX
			
			Integer( const integer& i ) : Value( (const vu_ibox&) i, V_INT ) {}
			
		#undef V_INT
			
			operator const bignum::integer&() const  { return number(); }
	};
	
	extern const type_info integer_vtype;
	
}

#endif
