/*
	dyad.hh
	-------
*/

#ifndef VC_DYAD_HH
#define VC_DYAD_HH

// plus
#include "plus/integer.hh"

// vc
#include "vc/op_type.hh"
#include "vc/value.hh"


namespace vc
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
