/*
	conditional.cc
	--------------
*/

#include "conditional.hh"


namespace tool
{
	
	static unsigned global_n_enabled_levels  = 0;
	static unsigned global_n_disabled_levels = 0;
	
	
	unsigned get_conditional_depth()
	{
		return global_n_enabled_levels + global_n_disabled_levels;
	}
	
	bool get_conditional_top()
	{
		return global_n_disabled_levels == 0;
	}
	
	void push_conditional( bool value )
	{
		if ( value )
		{
			++global_n_enabled_levels;
		}
		else
		{
			++global_n_disabled_levels;
		}
	}
	
	bool pop_conditional()
	{
		const bool top = get_conditional_top();
		
		if ( global_n_disabled_levels )
		{
			--global_n_disabled_levels;
		}
		else
		{
			--global_n_enabled_levels;
		}
		
		return top;
	}
	
}
