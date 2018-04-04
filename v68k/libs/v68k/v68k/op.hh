/*
	op.hh
	-----
*/

#ifndef V68K_OP_HH
#define V68K_OP_HH


namespace v68k
{
	
	enum op_result
	{
		Ok,
		
		Breakpoint = -1,
		
		Bus_error           = -2,
		Address_error       = -3,
		Illegal_instruction = -4,
		Division_by_zero    = -5,
		CHK_exception       = -6,
		Overflow_trap       = -7,
		Trace_exception     = -9,
		Format_error        = -14,
		
		Trap_0 = -32,
		Trap_1 = -33,
		Trap_2 = -34,
		Trap_3 = -35,
		Trap_4 = -36,
		Trap_5 = -37,
		Trap_6 = -38,
		Trap_7 = -39,
		Trap_8 = -40,
		Trap_9 = -41,
		Trap_A = -42,
		Trap_B = -43,
		Trap_C = -44,
		Trap_D = -45,
		Trap_E = -46,
		Trap_F = -47,
	};
	
}

#endif
