/*
	exceptions.hh
	-------------
*/

#ifndef RELIX_GLUE_EXCEPTIONS_HH
#define RELIX_GLUE_EXCEPTIONS_HH


namespace relix
{
	
	void bus_error();
	void address_error();
	void illegal_instruction();
	void division_by_zero();
	void integer_range_check();
	void integer_overflow();
	void privilege_violation();
	
}

#endif
