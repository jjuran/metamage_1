/*
	range.cc
	--------
*/

#include "vlib/types/range.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	static
	void check_range_operands( const Value& left, const Value& right )
	{
		if ( left.type() != Value_number  &&  left.type() != Value_byte )
		{
			THROW( "non-integer range operand" );
		}
		
		if ( right.type() != left.type() )
		{
			THROW( "type-mismatched range operands" );
		}
	}
	
	static
	size_t byterange_bin_size( const Value& v )
	{
		Expr* expr = v.expr();
		
		const uint8_t lower = expr->left .number().clipped();
		const uint8_t upper = expr->right.number().clipped();
		
		return upper - lower + (expr->op == Op_gamut);
	}
	
	static
	char* byterange_bin_copy( char* p, const Value& v )
	{
		Expr* expr = v.expr();
		
		uint8_t lower = expr->left .number().clipped();
		uint8_t upper = expr->right.number().clipped();
		
		if ( upper - lower + (expr->op == Op_gamut) == 0 )
		{
			return p;
		}
		
		if ( expr->op != Op_gamut )
		{
			--upper;
		}
		
		uint8_t i = lower;
		
		do
		{
			*p++ = i;
		}
		while ( i++ < upper );
		
		return p;
	}
	
	static const stringify byterange_bin =
	{
		NULL,
		&byterange_bin_size,
		&byterange_bin_copy,
	};
	
	static const stringifiers byterange_stringifiers =
	{
		NULL,            // str: undefined
		NULL,            // rep: undefined
		&byterange_bin,  // bin: packed array of bytes in sequence
	};
	
	const dispatch byterange_dispatch =
	{
		&byterange_stringifiers,
	};
	
	static inline
	const dispatch* get_dispatch( const Value& v )
	{
		return v.type() == Value_byte ? &byterange_dispatch : NULL;
	}
	
	Range::Range( const Value& left, op_type op, const Value& right )
	:
		Value( left, op, right, get_dispatch( left ) )
	{
		check_range_operands( left, right );
	}
	
}
