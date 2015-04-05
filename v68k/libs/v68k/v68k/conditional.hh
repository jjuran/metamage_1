/*
	conditional.hh
	--------------
*/

#ifndef V68K_CONDITIONAL_HH
#define V68K_CONDITIONAL_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	bool test_conditional( uint16_t condition, uint16_t nzvc );
	
}

#endif
