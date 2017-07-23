/*
	fraction.hh
	-----------
*/

#ifndef VLIB_TYPES_FRACTION_HH
#define VLIB_TYPES_FRACTION_HH

// bignum
#include "bignum/fraction.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Integer;
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch fraction_dispatch;
	
	class Fraction : public Value
	{
		public:
			static bool test( const Value& v );
			
			static Value coerce( const Value& v );
			
			Fraction( const bignum::fraction& f );
			
			typedef const Integer& Ref;
			
			const Integer& numerator  () const  { return (Ref) expr()->left;  }
			const Integer& denominator() const  { return (Ref) expr()->right; }
	};
	
	inline
	bool Fraction::test( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_divide;
		}
		
		return false;
	}
	
	extern const type_info fraction_vtype;
	
}

#endif
