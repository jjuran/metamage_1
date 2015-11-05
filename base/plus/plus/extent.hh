/*
	extent.hh
	---------
*/

#ifndef PLUS_EXTENT_HH
#define PLUS_EXTENT_HH


namespace plus
{
	
	typedef void (*destructor)( void* );
	
	char* extent_alloc( unsigned long capacity );
	
	void extent_add_ref( const char* buffer );
	void extent_release( const char* buffer );
	
	char* extent_unshare( char* buffer );
	
	void extent_set_destructor( const char* buffer, destructor dtor );
	
	unsigned long extent_refcount( const char* buffer );
	
	unsigned long extent_area( const char* buffer );
	
}

#endif
