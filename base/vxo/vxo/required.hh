/*
	required.hh
	-----------
*/

#ifndef VXO_REQUIRED_HH
#define VXO_REQUIRED_HH

// Standard C++
#include <new>


namespace vxo
{
	
	template < class T >
	T* required( T* p )
	{
		if ( ! p )
		{
			throw std::bad_alloc();
		}
		
		return p;
	}
	
}

#endif
