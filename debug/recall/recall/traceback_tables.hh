/*	===================
 *	traceback_tables.hh
 *	===================
 */

#ifndef RECALL_TRACEBACKTABLES_HH
#define RECALL_TRACEBACKTABLES_HH

// recall
#include "recall/return_address.hh"


namespace recall
{
	
	typedef unsigned char Byte;
	
	struct traceback_table;
	
	const traceback_table* find_symbol_name( return_address_traceback addr );
	
	const Byte* get_symbol_string( const traceback_table* table );
	
}

#endif
