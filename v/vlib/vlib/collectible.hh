/*
	collectible.hh
	--------------
*/

#ifndef VLIB_COLLECTIBLE_HH
#define VLIB_COLLECTIBLE_HH


namespace vlib
{
	
	struct Target;
	class Symbol;
	class Value;
	
	bool target_is_collectible( const Target& target );
	
	void track_symbol_if_collectible( const Value& symbol );
	
	bool symbol_list_with_values_is_collectible( const Value& symlist );
	
}

#endif
