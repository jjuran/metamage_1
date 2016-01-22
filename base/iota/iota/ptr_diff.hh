/*
	iota/ptr_diff.hh
	----------------
*/

#ifndef IOTA_PTRDIFF_HH
#define IOTA_PTRDIFF_HH


namespace iota
{
	
	inline
	unsigned long ptr_diff( const void* begin, const void* end )
	{
		return + static_cast< const char* >( end   )
		       - static_cast< const char* >( begin );
	}
	
}

#endif
