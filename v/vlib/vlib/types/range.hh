/*
	range.hh
	--------
*/

#ifndef VLIB_TYPES_RANGE_HH
#define VLIB_TYPES_RANGE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Range : public Value
	{
		public:
			static bool test( const Value& v );
			
			Range( const Value& left, op_type op, const Value& right );
	};
	
	inline
	bool Range::test( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_gamut  ||  expr->op == Op_delta;
		}
		
		return false;
	}
	
}

#endif
