/*
	ops.cc
	------
*/

#include "vlib/ops.hh"


namespace vlib
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
		{ Token_plus_x2,  Op_preinc },
		{ Token_minus_x2, Op_predec },
		{ Token_parens_plus, Op_unary_count },
		{ Token_asterisk,    Op_unary_deref },
	};
	
	const op_mapping when_value_acquired[] =
	{
		{ Token_plus_x2,  Op_postinc },
		{ Token_minus_x2, Op_postdec },
		
		{ Token_dot_punct, Op_named_unary },
		
		{ Token_dot,         Op_member    },
		{ Token_plus,        Op_add       },
		{ Token_minus,       Op_subtract  },
		{ Token_asterisk,    Op_multiply  },
		{ Token_slash,       Op_divide    },
		{ Token_percent,     Op_percent   },
		{ Token_caret,       Op_empower   },
		{ Token_parens_star, Op_repeat    },
		{ Token_equals_x2,   Op_equal     },
		{ Token_bang_equals, Op_unequal   },
		{ Token_lt,          Op_lt        },
		{ Token_lt_equals,   Op_lte       },
		{ Token_gt,          Op_gt        },
		{ Token_gt_equals,   Op_gte       },
		{ Token_equals_gt,   Op_mapping   },
		{ Token_colon,       Op_mapping   },
		{ Token_dot_x2,      Op_gamut     },
		{ Token_minus_gt,    Op_delta     },
		{ Token_comma,       Op_list      },
		{ Token_equals,      Op_duplicate },
		
		{ Token_colon_equals,    Op_approximate },
		
		{ Token_plus_equals,     Op_increase_by },
		{ Token_minus_equals,    Op_decrease_by },
		{ Token_asterisk_equals, Op_multiply_by },
		{ Token_slash_equals,    Op_divide_by   },
		{ Token_percent_equals,  Op_percent_by  },
		
		{ Token_lt_minus_x2, Op_push },
		
		{ Token_ampersand, Op_intersection },
		{ Token_pipe,      Op_union        },
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
