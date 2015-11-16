/*
	op_type.hh
	----------
*/

#ifndef VLIB_OPTYPE_HH
#define VLIB_OPTYPE_HH


namespace vlib
{
	
	enum op_type
	{
		Op_none,
		Op_named_unary,
		Op_const,
		Op_var,
		Op_unary_plus,
		Op_unary_minus,
		Op_unary_count,
		Op_duplicate,
		Op_increase_by,
		Op_decrease_by,
		Op_multiply_by,
		Op_divide_by,
		Op_percent_by,
		Op_remain_by = Op_percent_by,
		Op_denote,
		Op_empower,
		Op_function,
		Op_member,
		Op_multiply,
		Op_divide,
		Op_percent,
		Op_remain    = Op_percent,
		Op_bind_args = Op_percent,
		Op_modulo,
		Op_add,
		Op_subtract,
		Op_repeat,  // multiply list
		Op_equal,
		Op_unequal,
		Op_lt,
		Op_lte,
		Op_gt,
		Op_gte,
		Op_list,
		Op_parens,
		Op_end,
	};
	
	inline
	bool declares_symbols( op_type op )
	{
		return op == Op_const  ||  op == Op_var;
	}
	
	inline
	bool is_left_varop( op_type op )
	{
		return op >= Op_duplicate  &&  op < Op_empower;
	}
	
}

#endif
