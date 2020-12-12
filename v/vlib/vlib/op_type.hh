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
	// begin right-associative
		
		Op_reserved = -1,
		Op_none,
		Op_return,
		Op_throw,
		Op_break,
		Op_continue,
		Op_record,
		Op_module,
		Op_export,
		
	// begin declares-symbols
		Op_const,
		Op_let,
		Op_var,
		Op_def,
		Op_present,
		Op_for,
	// end declares-symbols
		
	// begin left-var
		Op_move,
		Op_swap,
		Op_unary_refer,
		Op_take,
		Op_preinc,
		Op_predec,
		Op_postinc,
		Op_postdec,
		Op_duplicate,
		Op_approximate,
		Op_concat_with,
		Op_increase_by,
		Op_decrease_by,
		Op_multiply_by,
		Op_divide_by,
		Op_div_int_by,
		Op_percent_by,
		Op_remain_by = Op_percent_by,
		Op_setAND_with,
		Op_setXOR_with,
		Op_setOR_with,
		Op_push,
		
		Op_denote,
		
	// end left-var
		Op_empower,
		Op_named_unary,
	// end right-associative
		
		Op_function,
		
		Op_typeof,
		Op_unary_plus,
		Op_unary_minus,
		Op_unary_negate,
		Op_unary_count,
		Op_unary_deref,
		Op_postfix_0_1,
		Op_postfix_0_N,
		Op_postfix_1_N,
		Op_forward_init,
		Op_reverse_init,
		Op_via,
		Op_auto,
		Op_begin,
		Op_array,
		Op_subscript,
		Op_block,
		Op_member,
		Op_getter,
		Op_multiply,
		Op_divide,
		Op_DIV,
		Op_divide_int = Op_DIV,  // integer division
		Op_percent,
		Op_remain    = Op_percent,
		Op_format    = Op_percent,
		Op_bind_args = Op_percent,
		Op_modulo,
		Op_add,
		Op_subtract,
		Op_intersection,
		Op_exclusion,
		Op_union,
		Op_in,
		Op_not,
		Op_repeat,  // multiply list
		Op_gap,
		Op_map,
		Op_pop,
		Op_ver,
		Op_per,
		Op_isa,
		Op_equal,
		Op_unequal,
		Op_lt,
		Op_lte,
		Op_gt,
		Op_gte,
		Op_cmp,
		
	// begin right-associative
		Op_mapping,
		Op_named_param,
		Op_gamut,
		Op_delta,
		Op_and,
		Op_or,
		Op_list,
		Op_send,
		Op_recv,
		Op_assert,
		Op_lambda,
		Op_try,
		Op_catch,
		Op_if,
		Op_then,
		Op_else,
		Op_while,    // `while` statement
		Op_while_2,  // `while` clause of `do` statement
		Op_do,    // `do` statement
		Op_do_2,  // `do` clause of `while` statement
		Op_parens,
		Op_brackets,
		Op_braces,
		Op_prototype,
		Op_internal_vector,
		Op_keyvar,
		Op_contains,
		
		Op_end,
	// end right-associative
		
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
		return op >= Op_const  &&  op <= Op_for;
	}
	
	inline
	bool is_left_varop( op_type op )
	{
		return op >= Op_move  &&  op < Op_empower;
	}
	
	inline
	bool is_right_associative( op_type op )
	{
		if ( op == Op_move )
		{
			return false;
		}
		
		return op <= Op_named_unary  ||  (op >= Op_mapping  &&  op <= Op_end);
	}
	
	inline
	bool is_right_varop( op_type op )
	{
		return op >= Op_move  &&  op <= Op_unary_refer;
	}
	
	inline
	bool is_right_unary( op_type op )
	{
		switch ( op )
		{
			case Op_postinc:
			case Op_postdec:
			case Op_present:
			case Op_postfix_0_1:
			case Op_postfix_0_N:
			case Op_postfix_1_N:
				return true;
			
			default:
				return false;
		}
	}
	
	inline
	bool is_supporting( op_type op )
	{
		switch ( op )
		{
			case Op_named_param:
			case Op_forward_init:
			case Op_reverse_init:
			case Op_via:
			case Op_mapping:
			case Op_then:
			case Op_else:
			case Op_do_2:
			case Op_while_2:
			case Op_catch:
			case Op_scope:
			case Op_prototype:
				return true;
			
			default:
				return false;
		}
	}
	
}

#endif
