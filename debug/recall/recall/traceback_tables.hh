/*	===================
 *	traceback_tables.hh
 *	===================
 */

#ifndef RECALL_TRACEBACKTABLES_HH
#define RECALL_TRACEBACKTABLES_HH

// plus
#include "plus/string.hh"

// Recall
#include "recall/return_address.hh"


namespace recall
{
	
	struct traceback_table;
	
	const traceback_table* find_symbol_name( return_address_traceback addr );
	
	plus::string get_symbol_string( const traceback_table* table );
	
}

#endif

