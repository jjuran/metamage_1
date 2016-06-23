/*
	collectible.hh
	--------------
*/

#ifndef VLIB_COLLECTIBLE_HH
#define VLIB_COLLECTIBLE_HH


namespace vlib
{
	
	class Symbol;
	class Value;
	
	bool symbol_is_collectible( const Symbol& symbol, const Value& v );

	bool symbol_list_is_collectible( const Value& symlist );
	
}

#endif
