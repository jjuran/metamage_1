/*
	datum_alloc.hh
	--------------
*/

#ifndef PLUS_DATUMALLOC_HH
#define PLUS_DATUMALLOC_HH


namespace plus
{
	
	enum delete_policy
	{
		delete_never,  // propagates, for static storage like argv members
		delete_shared, // Refcounted delete, for everything by default
		delete_owned,  // Stored as shared, but can't be shared again
		delete_basic,  // Standard-issue delete, for caller-supplied handoffs
		delete_free    // Calls free(), not operator delete()
	};
	
	struct datum_storage;
	
	char* datum_alloc( unsigned long size );
	
	void datum_free( char* mem );
	
	char* allocate( datum_storage& datum, long length, long capacity );
	
	inline char* allocate( datum_storage& datum, long length )
	{
		return allocate( datum, length, length );
	}
	
	char* allocate_data( datum_storage& datum, const char* p, long n );
	
	void destroy( datum_storage& datum );
	
	char* reallocate( datum_storage& datum, long length );
	
	char* extend_capacity( datum_storage& datum, long new_capacity );
	
	char* curtail_capacity( datum_storage& datum, long new_capacity );
	
}

#endif

