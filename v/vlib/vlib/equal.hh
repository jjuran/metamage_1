/*
	equal.hh
	--------
*/

#ifndef VLIB_EQUAL_HH
#define VLIB_EQUAL_HH


namespace vlib
{
	
	class Value;
	
	bool single_type_mismatch( const Value& one, const Value& two );
	
	bool equal( const Value& one, const Value& two );
	
}

#endif
