/*
	conditional.hh
	--------------
*/

#ifndef MXCPP_CONDITIONAL_HH
#define MXCPP_CONDITIONAL_HH


namespace tool
{
	
	class unmatched_endif {};
	
	unsigned get_conditional_depth();
	
	bool get_conditional_top();
	
	void push_conditional( bool value );
	
	bool pop_conditional();
	
}

#endif
