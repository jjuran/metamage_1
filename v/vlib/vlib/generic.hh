/*
	generic.hh
	----------
*/

#ifndef VLIB_GENERIC_HH
#define VLIB_GENERIC_HH


namespace vlib
{
	
	template < class T >
	void generic_destructor( void* pointer )
	{
		T* obj = (T*) pointer;
		
		obj->~T();
	}
	
}

#endif
