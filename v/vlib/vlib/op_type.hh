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
		Op_unary_deref,
		Op_preinc,
		Op_predec,
		Op_postinc,
		Op_postdec,
		Op_duplicate,
		Op_approximate,
		Op_increase_by,
		Op_decrease_by,
		Op_multiply_by,
		Op_divide_by,
		Op_percent_by,
		Op_remain_by = Op_percent_by,
		Op_denote,
		Op_empower,
		Op_function,
		Op_array,
		Op_subscript,
		Op_block,
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
		Op_map,
		Op_isa,
		Op_equal,
		Op_unequal,
		Op_lt,
		Op_lte,
		Op_gt,
		Op_gte,
		Op_list,
		Op_if,
		Op_then,
		Op_else,
		Op_while,
		Op_do,
		Op_break,
		Op_continue,
		Op_parens,
		Op_brackets,
		Op_braces,
		Op_end,
		
		Op_invocation = 0x100 | Op_block,
	};
	
	inline
	bool declares_symbols( op_type op )
	{
		return op == Op_const  ||  op == Op_var;
	}
	
	inline
	bool is_left_varop( op_type op )
	{
		return op >= Op_preinc  &&  op < Op_empower;
	}
	
	inline
	bool is_right_unary( op_type op )
	{
		return op == Op_postinc  ||  op == Op_postdec;
	}
	
	inline
	bool is_supporting( op_type op )
	{
		switch ( op )
		{
			case Op_then:
			case Op_else:
			case Op_do:
				return true;
			
			default:
				return false;
		}
	}
	
}

#endif
