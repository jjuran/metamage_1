/*
	binary_ops.hh
	-------------
*/

#ifndef MXCPP_BINARYOPS_HH
#define MXCPP_BINARYOPS_HH

// mxcpp
#include "value.hh"


namespace tool
{
	
	typedef value_t (*binary_operator)( const value_t& a, const value_t& b );
	
	struct binary_operation
	{
		const char*      name;
		int              rank;
		binary_operator  code;
	};
	
	enum
	{
		select_rank = -13
	};
	
	inline bool is_right_associative( const binary_operation* op )
	{
		return op->rank == select_rank;
	}
	
	class bad_binary_operator {};
	
	const binary_operation* next_binary_operator( const char* p );
	
}

#endif
