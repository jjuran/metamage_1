/*
	extent.hh
	---------
*/

#ifndef PLUS_EXTENT_HH
#define PLUS_EXTENT_HH


namespace plus
{
	
	char* extent_alloc( unsigned long capacity );
	
	void extent_add_ref( const char* buffer );
	void extent_release( const char* buffer );
	
	unsigned long extent_refcount( const char* buffer );
}

#endif
