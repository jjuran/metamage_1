/*
	ops.cc
	------
*/

#include "vlib/ops.hh"

// iota
#include "iota/typed_enum.hh"


namespace vlib
{
	
	/*
		While parsing an expression, either a value is expected or one has
		just been acquired.
		
		If a value is expected, the next token may be a left unary operator
		or a nullary operator.  Otherwise, it may be a right unary operator
		or a binary operator.
	*/
	
	enum spacing_bits
	{
		Spacing_tight,  //   a:b
		Spacing_onlyR,  //   a: b
		Spacing_onlyL,  //  a :b
		Spacing_loose,  //  a : b
	};
	
	TYPED_ENUM( spacing, unsigned char )
	{
		Tight = 1 << Spacing_tight,
		OnlyR = 1 << Spacing_onlyR,
		OnlyL = 1 << Spacing_onlyL,
		Loose = 1 << Spacing_loose,
		
		Postfix = OnlyR,
		Match   = Tight | Loose,
		MathL   = Tight | OnlyL,
		
		Every = Tight | OnlyR | OnlyL | Loose,
		
		Not_L = Every - OnlyL,
		Not_R = Every - OnlyR,
	};
	
	struct op_mapping
	{
		token_type  token;
		spacing     space;
		op_type     op;
	};
	
	const op_mapping when_value_expected[] =
	{
		{ Token_dot,         OnlyL, Op_getter       },
		{ Token_colon,       OnlyL, Op_keyvar       },
		{ Token_plus,        MathL, Op_unary_plus   },
		{ Token_minus,       MathL, Op_unary_minus  },
		{ Token_tilde,       MathL, Op_unary_negate },
		{ Token_plus_x2,     MathL, Op_preinc       },
		{ Token_minus_x2,    MathL, Op_predec       },
		{ Token_lt_equals,   Not_R, Op_recv         },
		{ Token_parens_plus, Not_R, Op_unary_count  },
		{ Token_asterisk,    Not_R, Op_unary_deref  },
		{ Token_ampersand,   Not_R, Op_unary_refer  },
	};
	
	const op_mapping when_value_acquired[] =
	{
		{ Token_plus_x2,     Postfix, Op_postinc },
		{ Token_minus_x2,    Postfix, Op_postdec },
		{ Token_question_x2, Postfix, Op_present },
		
		{ Token_question, Postfix, Op_postfix_0_1 },
		{ Token_asterisk, Postfix, Op_postfix_0_N },
		{ Token_plus,     Postfix, Op_postfix_1_N },
		
		{ Token_dot,          Tight, Op_member    },
		{ Token_plus,         Tight, Op_reserved  },
		{ Token_plus,         Loose, Op_add       },
		{ Token_minus,        Tight, Op_reserved  },
		{ Token_minus,        Loose, Op_subtract  },
		{ Token_asterisk,     Loose, Op_multiply  },
		{ Token_slash,        Match, Op_divide    },
		{ Token_percent,      Match, Op_percent   },
		{ Token_caret,        Every, Op_empower   },
		{ Token_parens_star,  Match, Op_repeat    },
		{ Token_equals_x2,    Loose, Op_equal     },
		{ Token_bang_equals,  Loose, Op_unequal   },
		{ Token_lt,           Loose, Op_lt        },
		{ Token_lt_equals,    Loose, Op_lte       },
		{ Token_lt_equals_gt, Loose, Op_cmp       },
		{ Token_gt,           Loose, Op_gt        },
		{ Token_gt_equals,    Loose, Op_gte       },
		{ Token_equals_gt,    Loose, Op_mapping   },
		{ Token_colon,        Tight, Op_mapping   },
		{ Token_colon,        OnlyR, Op_mapping   },
		{ Token_colon,        Loose, Op_reserved  },
		{ Token_dot_x2,       Match, Op_gamut     },
		{ Token_minus_gt,     Match, Op_delta     },
		{ Token_comma,        Every, Op_list      },
		{ Token_equals,       Loose, Op_duplicate },
		{ Token_equals,       Tight, Op_named_param},
		
		{ Token_lt_minus,     Loose, Op_move },
		{ Token_lt_minus_gt,  Loose, Op_swap },
		
		{ Token_colon_equals, Loose, Op_approximate },
		
		{ Token_dot_equals,         Loose, Op_concat_with },
		{ Token_plus_equals,        Loose, Op_increase_by },
		{ Token_minus_equals,       Loose, Op_decrease_by },
		{ Token_asterisk_equals,    Loose, Op_multiply_by },
		{ Token_slash_equals,       Loose, Op_divide_by   },
		{ Token_slash_colon_equals, Loose, Op_div_int_by  },
		{ Token_percent_equals,     Loose, Op_percent_by  },
		{ Token_ampersand_equals,   Loose, Op_setAND_with },
		{ Token_tilde_equals,       Loose, Op_setXOR_with },
		{ Token_pipe_equals,        Loose, Op_setOR_with  },
		
		{ Token_lt_minus_x2,  Loose, Op_push },
		{ Token_lt_equals_x2, Loose, Op_send },
		
		{ Token_gt_minus,    Every, Op_forward_init },
		{ Token_minus_lt,    Every, Op_reverse_init },
		
		{ Token_ampersand,   Match, Op_intersection },
		{ Token_tilde,       Match, Op_exclusion    },
		{ Token_pipe,        Match, Op_union        },
	};
	
	static inline
	spacing token_spacing_mask( const Token& token )
	{
		return spacing( 1 << (token.space_before * 2 + token.space_after) );
	}
	
	static
	const op_mapping* find( const op_mapping* a, short n, const Token& token )
	{
		while ( n-- )
		{
			if ( a->token == token  &&  a->space & token_spacing_mask( token ) )
			{
				return a;
			}
			
			++a;
		}
		
		return 0;  // NULL
	}
	
	#define FIND( array, x )  find( array, sizeof array / sizeof array[0], (x) )
	
	static
	const op_mapping* find_token_op( const Token& token, bool expecting_value )
	{
		return expecting_value ? FIND( when_value_expected, token )
		                       : FIND( when_value_acquired, token );
	}
	
	op_type op_from_token( const Token& token, bool expecting_value )
	{
		if ( const op_mapping* it = find_token_op( token, expecting_value ) )
		{
			return it->op;
		}
		
		return Op_none;
	}
	
}
