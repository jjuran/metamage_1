/*
	iota/swap.hh
	------------
	
	Joshua Juran
*/

#ifndef IOTA_SWAP_HH
#define IOTA_SWAP_HH

// Standard C++
#ifdef __GNUC__
#include <algorithm>
#endif


namespace iota
{
	
#ifdef __GNUC__
	
	// Something is bringing std::swap() into scope, making swap() ambiguous
	// unless iota::swap() and std::swap() are the same.
	
	using std::swap;
	
#else
	
	// Write out own swap() and save the cost of including <algorithm>
	
	template < class T >
	inline void swap( T& a, T& b )
	{
		T temp = a;
		
		a = b;
		
		b = temp;
	}
	
#endif
	
}

#endif

