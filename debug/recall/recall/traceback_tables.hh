/*	===================
 *	traceback_tables.hh
 *	===================
 */

#ifndef RECALL_TRACEBACKTABLES_HH
#define RECALL_TRACEBACKTABLES_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct traceback_table;
	
	const traceback_table* find_symbol_name( return_address_traceback addr );
	
	std::string get_symbol_string( const traceback_table* table );
	
}

#endif

