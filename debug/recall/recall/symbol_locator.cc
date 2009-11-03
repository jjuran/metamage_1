/*
	symbol_locator.cc
	-----------------
*/

#include "recall/symbol_locator.hh"


namespace recall
{
	
	static symbol_locator global_symbol_locator = 0;  // NULL
	
	void set_symbol_locator( symbol_locator f )
	{
		global_symbol_locator = f;
	}
	
	symbol_locator get_symbol_locator()
	{
		return global_symbol_locator;
	}
	
}

