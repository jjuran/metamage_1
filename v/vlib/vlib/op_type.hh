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
		Op_intersection,
		Op_union,
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
		
		Op_frame,  // links caller's stack to a new stack frame
		Op_scope,  // links a set of symbols to the code declaring them
		
		/*
			An invocation is a transformed block.  The dummy left operand is
			replaced with `&proc_invoke` (a use of dependency injection that
			prevents `calc` from depending on `execute`), and the scope is
			replaced with an activation.  (Actually, an invocation's right
			operand could also be a non-block expression.)
			
			An activation is a transformed scope.  The placeholder `_` is
			replaced with the one in the current stack frame, though this
			value is only used when entering a scope -- if a block is called
			with arguments, they replace the value in `_`.  The new set of
			symbols (i.e. `_` and the local presets) become a new stack frame
			and the new stack replaces the symbols as the left operand.
			
			An expression also binds code to a stack (and is further bound
			to proc_invoke in an invocation), but it doesn't create a lexical
			scope.  The one example so far is the condition of while-do.
		*/
		
		Op_expression,
		
		Op_invocation = 0x100 | Op_block,
		Op_activation = 0x100 | Op_scope,
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
			case Op_scope:
				return true;
			
			default:
				return false;
		}
	}
	
}

#endif
