/*
	precedence.cc
	-------------
*/

#include "vlib/precedence.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vlib
{
	
	enum precedence_level
	{
		Precedence_none = 0,
		
		Precedence_declarator,      // const var def
		Precedence_adjacency,       // f(x) v[i] ?? foo.bar .bar
		Precedence_unary_step,      // ++ --
		Precedence_take,            // take
		Precedence_exponentiation,  // ^
		Precedence_unary_math,      // + - ~ * & (+) <= ? * +
		Precedence_initial_value,   // >- -< via
		Precedence_unary_name,      // (f x) auto begin lambda x=k
		Precedence_multiplication,  // * /
		Precedence_addition,        // + -
		Precedence_range,           // .. ->
		Precedence_intersection,    // &
		Precedence_exclusion,       // ~
		Precedence_union,           // |
		Precedence_in,              // in
		Precedence_not,             // not
		Precedence_repeat,          // (*)
		Precedence_try,             // try catch
		Precedence_map,             // map gap ver per
		Precedence_comparison,      // <=>
		Precedence_inequality,      // < <= > >=
		Precedence_equality,        // == !=
		Precedence_and,             // and
		Precedence_or,              // or
		Precedence_mapping,         // => :
		Precedence_list,            // ,
		Precedence_move,            // <- <->
		Precedence_assignment,      // = := .= += -= *= /= /:= %= <-- <==
		Precedence_conditional,     // if then else while do for
		Precedence_command,         // assert return throw
		Precedence_end,             // ;
		Precedence_group,           // (
	};
	
	struct precedence_mapping
	{
		precedence_level  precedence;
		op_type           op;
	};
	
	static const precedence_mapping precedence_table[] =
	{
		{ Precedence_declarator, Op_module },
		{ Precedence_declarator, Op_export },
		{ Precedence_declarator, Op_const },
		{ Precedence_declarator, Op_var   },
		{ Precedence_declarator, Op_def   },
		
		{ Precedence_adjacency, Op_function  },
		{ Precedence_adjacency, Op_subscript },
		{ Precedence_adjacency, Op_member    },
		{ Precedence_adjacency, Op_getter    },
		{ Precedence_adjacency, Op_denote    },
		{ Precedence_adjacency, Op_present   },
		
		{ Precedence_unary_step, Op_preinc  },
		{ Precedence_unary_step, Op_predec  },
		{ Precedence_unary_step, Op_postinc },
		{ Precedence_unary_step, Op_postdec },
		
		{ Precedence_take, Op_take },
		
		{ Precedence_exponentiation, Op_empower },
		
		{ Precedence_unary_math, Op_recv        },
		{ Precedence_unary_math, Op_unary_plus  },
		{ Precedence_unary_math, Op_unary_minus },
		{ Precedence_unary_math, Op_unary_negate },
		{ Precedence_unary_math, Op_unary_count },
		{ Precedence_unary_math, Op_unary_deref },
		{ Precedence_unary_math, Op_unary_refer },
		
		{ Precedence_unary_math, Op_postfix_0_1 },
		{ Precedence_unary_math, Op_postfix_0_N },
		{ Precedence_unary_math, Op_postfix_1_N },
		
		{ Precedence_initial_value, Op_forward_init },
		{ Precedence_initial_value, Op_reverse_init },
		{ Precedence_initial_value, Op_via          },
		
		{ Precedence_unary_name, Op_auto        },
		{ Precedence_unary_name, Op_begin       },
		{ Precedence_unary_name, Op_lambda      },
		{ Precedence_unary_name, Op_named_unary },
		{ Precedence_unary_name, Op_named_param },
		
		{ Precedence_multiplication, Op_multiply },
		{ Precedence_multiplication, Op_divide   },
		{ Precedence_multiplication, Op_DIV      },
		{ Precedence_multiplication, Op_percent  },
		{ Precedence_multiplication, Op_modulo   },
		
		{ Precedence_addition, Op_add      },
		{ Precedence_addition, Op_subtract },
		
		{ Precedence_range, Op_gamut },
		{ Precedence_range, Op_delta },
		
		{ Precedence_intersection, Op_intersection },
		{ Precedence_exclusion,    Op_exclusion    },
		{ Precedence_union,        Op_union        },
		
		{ Precedence_in, Op_in },
		
		{ Precedence_not, Op_not },
		
		{ Precedence_repeat, Op_repeat },
		
		{ Precedence_try, Op_try   },
		{ Precedence_try, Op_catch },
		
		{ Precedence_map, Op_map },
		{ Precedence_map, Op_gap },
		{ Precedence_map, Op_ver },
		{ Precedence_map, Op_per },
		
		{ Precedence_comparison, Op_cmp },
		
		{ Precedence_inequality, Op_lt  },
		{ Precedence_inequality, Op_lte },
		{ Precedence_inequality, Op_gt  },
		{ Precedence_inequality, Op_gte },
		
		{ Precedence_equality, Op_isa     },
		{ Precedence_equality, Op_equal   },
		{ Precedence_equality, Op_unequal },
		
		{ Precedence_and, Op_and },
		{ Precedence_or,  Op_or  },
		
		{ Precedence_mapping, Op_mapping },
		
		{ Precedence_list, Op_list },
		
		{ Precedence_move, Op_move },
		{ Precedence_move, Op_swap },
		
		{ Precedence_assignment, Op_duplicate   },
		{ Precedence_assignment, Op_approximate },
		{ Precedence_assignment, Op_concat_with },
		{ Precedence_assignment, Op_increase_by },
		{ Precedence_assignment, Op_decrease_by },
		{ Precedence_assignment, Op_multiply_by },
		{ Precedence_assignment, Op_divide_by   },
		{ Precedence_assignment, Op_div_int_by  },
		{ Precedence_assignment, Op_percent_by  },
		{ Precedence_assignment, Op_push        },
		{ Precedence_assignment, Op_send        },
		
		{ Precedence_conditional, Op_if   },
		{ Precedence_conditional, Op_then },
		{ Precedence_conditional, Op_else },
		
		{ Precedence_conditional, Op_for     },
		{ Precedence_conditional, Op_while   },
		{ Precedence_conditional, Op_do      },
		{ Precedence_conditional, Op_do_2    },
		{ Precedence_conditional, Op_while_2 },
		
		{ Precedence_command, Op_assert },
		{ Precedence_command, Op_return },
		{ Precedence_command, Op_throw  },
		
		{ Precedence_end, Op_end },
		
		{ Precedence_group, Op_parens   },
		{ Precedence_group, Op_brackets },
		{ Precedence_group, Op_braces   },
	};
	
	static precedence_level op_precedence( op_type op )
	{
		const precedence_mapping* begin = precedence_table;
		const precedence_mapping* end   = ARRAY_END( precedence_table );
		
		for ( const precedence_mapping* it = begin;  it < end;  ++it )
		{
			if ( it->op == op )
			{
				return it->precedence;
			}
		}
		
		return Precedence_none;
	}
	
	bool decreasing_op_precedence( op_type left, op_type right )
	{
		const precedence_level pr_left  = op_precedence( left  );
		const precedence_level pr_right = op_precedence( right );
		
		/*
			Precedence levels start at 1 and *increase* numerically with
			*lower* precedence, so decreasing precedence means the right
			is greater than the left.
			
			If the operator is right-associative, then fudge the level so
			the left is lower-precedence than the right.
		*/
		
		return pr_right >= pr_left + is_right_associative( right );
	}
	
}
