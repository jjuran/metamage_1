/*
	block-utils.hh
	--------------
*/

#ifndef VLIB_BLOCKUTILS_HH
#define VLIB_BLOCKUTILS_HH


namespace vlib
{
	
	class Value;
	
	bool is_block( const Value& v );
	
	const Value& unembrace( const Value& block );
	
}

#endif
