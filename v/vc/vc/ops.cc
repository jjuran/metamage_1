/*
	ops.cc
	------
*/

#include "vc/ops.hh"


namespace vc
{
	
	/*
		While parsing an expression, either a value is expected or one has
		just been acquired.
		
		If a value is expected, the next token may be a left unary operator
		or a nullary operator.  Otherwise, it may be a right unary operator
		or a binary operator.
	*/
	
	struct op_mapping
	{
		token_type  token;
		op_type     op;
	};
	
	const op_mapping when_value_expected[] =
	{
		{ Token_plus,  Op_unary_plus  },
		{ Token_minus, Op_unary_minus },
	};
	
	const op_mapping when_value_acquired[] =
	{
		{ Token_plus,     Op_add      },
		{ Token_minus,    Op_subtract },
		{ Token_asterisk, Op_multiply },
		{ Token_slash,    Op_divide   },
		{ Token_percent,  Op_remain   },
		{ Token_caret,    Op_empower  },
	};
	
	static
	const op_mapping* find( const op_mapping* a, unsigned n, token_type token )
	{
		while ( n-- )
		{
			if ( a->token == token )
			{
				return a;
			}
			
			++a;
		}
		
		return 0;  // NULL
	}
	
	#define FIND( array, x )  find( array, sizeof array / sizeof array[0], (x) )
	
	static
	const op_mapping* find_token_op( token_type token, bool expecting_value )
	{
		return expecting_value ? FIND( when_value_expected, token )
		                       : FIND( when_value_acquired, token );
	}
	
	op_type op_from_token( token_type token, bool expecting_value )
	{
		if ( const op_mapping* it = find_token_op( token, expecting_value ) )
		{
			return it->op;
		}
		
		return Op_none;
	}
	
}
