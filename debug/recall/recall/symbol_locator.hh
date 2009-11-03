/*
	symbol_locator.hh
	-----------------
*/

#ifndef RECALL_SYMBOLLOCATOR_HH
#define RECALL_SYMBOLLOCATOR_HH


namespace recall
{
	
	typedef const char* (*symbol_locator)( const void* address );
	
	void set_symbol_locator( symbol_locator f );
	
	symbol_locator get_symbol_locator();
	
}

#endif

