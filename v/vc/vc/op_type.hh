/*
	op_type.hh
	----------
*/

#ifndef VC_OPTYPE_HH
#define VC_OPTYPE_HH


namespace vc
{
	
	enum op_type
	{
		Op_none,
		Op_function,
		Op_const,
		Op_var,
		Op_unary_plus,
		Op_unary_minus,
		Op_duplicate,
		Op_empower,
		Op_multiply,
		Op_divide,
		Op_remain,
		Op_modulo,
		Op_add,
		Op_subtract,
		Op_equal,
		Op_unequal,
		Op_lt,
		Op_lte,
		Op_gt,
		Op_gte,
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
		return op == Op_duplicate;
	}
	
}

#endif
