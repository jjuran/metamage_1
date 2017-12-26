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
	
	bool symbol_is_collectible( const Symbol& symbol );

	bool symbol_list_is_collectible( const Value& symlist );
	
}

#endif
