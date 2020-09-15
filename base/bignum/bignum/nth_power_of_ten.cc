/*
	nth_power_of_ten.cc
	-------------------
*/

#include "bignum/nth_power_of_ten.hh"

// Standard C++
#include <vector>


namespace bignum
{
	
	static std::vector< integer > memoized_powers_of_ten;
	
	
	const integer& nth_power_of_ten( unsigned n )
	{
		if ( n >= memoized_powers_of_ten.size() )
		{
			const integer ten = 10;
			
			if ( memoized_powers_of_ten.empty() )
			{
				memoized_powers_of_ten.push_back(   1 );
				memoized_powers_of_ten.push_back( ten );
			}
			
			for ( unsigned i = memoized_powers_of_ten.size();  i <= n;  ++i )
			{
				const integer& last = memoized_powers_of_ten.back();
				
				memoized_powers_of_ten.push_back( last * ten );
			}
		}
		
		return memoized_powers_of_ten[ n ];
	}
	
}
