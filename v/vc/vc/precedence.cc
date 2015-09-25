/*
	precedence.cc
	-------------
*/

#include "vc/precedence.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vc
{
	
	enum precedence_level
	{
		Precedence_none = 0,
		
		Precedence_exponentiation,  // ^
		Precedence_unary_math,      // + -
		Precedence_multiplication,  // * /
		Precedence_addition,        // + -
		Precedence_inequality,      // < <= > >=
		Precedence_equality,        // == !=
		Precedence_assignment,      // =
		Precedence_end,             // ;
	};
	
	struct precedence_mapping
	{
		precedence_level  precedence;
		op_type           op;
	};
	
	static const precedence_mapping precedence_table[] =
	{
		{ Precedence_exponentiation, Op_empower },
		
		{ Precedence_unary_math, Op_function    },
		{ Precedence_unary_math, Op_const       },
		{ Precedence_unary_math, Op_var         },
		{ Precedence_unary_math, Op_unary_plus  },
		{ Precedence_unary_math, Op_unary_minus },
		
		{ Precedence_multiplication, Op_multiply },
		{ Precedence_multiplication, Op_divide   },
		{ Precedence_multiplication, Op_remain   },
		{ Precedence_multiplication, Op_modulo   },
		
		{ Precedence_addition, Op_add      },
		{ Precedence_addition, Op_subtract },
		
		{ Precedence_inequality, Op_lt  },
		{ Precedence_inequality, Op_lte },
		{ Precedence_inequality, Op_gt  },
		{ Precedence_inequality, Op_gte },
		
		{ Precedence_equality, Op_equal   },
		{ Precedence_equality, Op_unequal },
		
		{ Precedence_assignment, Op_duplicate },
		
		{ Precedence_end, Op_end },
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
	
	static bool is_right_associative( op_type op )
	{
		return op <= Op_empower;
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
