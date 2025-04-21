/*	===================
 *	traceback_tables.hh
 *	===================
 */

#ifndef RECALL_TRACEBACKTABLES_HH
#define RECALL_TRACEBACKTABLES_HH

// more-libc
#include "more/string.h"

// recall
#include "recall/return_address.hh"


namespace recall
{
	
	typedef unsigned char Byte;
	
	struct traceback_table;
	
	inline
	const char* to_char( const Byte* s, char* buffer )
	{
		size_t n = *s++;
		
		char* p = (char*) mempcpy( buffer, s, n );
		
		*p = '\0';
		
		return buffer;
	}
	
	const traceback_table* find_symbol_name( return_address_traceback addr );
	
	const Byte* get_symbol_string( const traceback_table* table );
	
}

#endif
