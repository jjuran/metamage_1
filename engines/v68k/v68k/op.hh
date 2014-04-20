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
		
		Bus_error     = -2,
		Address_error = -3,
		Format_error  = -14,
	};
	
}

#endif
