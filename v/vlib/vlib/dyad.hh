/*
	dyad.hh
	-------
*/

#ifndef VLIB_DYAD_HH
#define VLIB_DYAD_HH

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/op_type.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct dyad
	{
		Value    v;
		op_type  op;
		
		dyad( op_type op ) : v( Value_dummy_operand ), op( op )
		{
		}
		
		dyad( const Value& v ) : v( v ), op()
		{
		}
		
		void swap( dyad& b );
	};
	
	inline void swap( dyad& a, dyad& b )
	{
		a.swap( b );
	}
	
}

#endif
