/*
	datum_alloc.hh
	--------------
*/

#ifndef PLUS_DATUMALLOC_HH
#define PLUS_DATUMALLOC_HH

// plus
#include "plus/datum_storage.hh"


namespace plus
{
	
	enum delete_policy
	{
		delete_never,  // propagates, for static storage like argv members
		delete_shared, // Refcounted delete, for everything by default
		delete_owned,  // Stored as shared, but can't be shared again
		delete_free    // Calls free(), not extent_release()
	};
	
	
	inline void construct_from_default( datum_storage& x )
	{
		x.small[ datum_max_offset ] = datum_max_offset;
	}
	
	void construct_from_move_untaint_policy( datum_storage&  x,
	                                         datum_movable&  y,
	                                         signed char     _old_policy );
	
	inline void construct_from_move_untaint( datum_storage& x, datum_movable& y )
	{
		construct_from_move_untaint_policy( x, y, ~delete_owned );
	}
	
	inline void construct_from_move( datum_storage& x, datum_movable& y )
	{
		construct_from_move_untaint_policy( x, y, 0 );
	}
	
	void assign_from_move( datum_storage& x, datum_movable& y );
	
	
	char* allocate_data( datum_storage& datum, const char* p, long n );
	
	void construct_from_copy( datum_storage& x, const datum_storage& y, bool taint = false );
	
	void assign_from_copy( datum_storage& x, const datum_storage& y, bool taint = false );
	
	inline void construct_from_copy_taint( datum_storage& x, const datum_storage& y )
	{
		construct_from_copy( x, y, true );
	}
	
	inline void assign_from_copy_taint( datum_storage& x, const datum_storage& y )
	{
		assign_from_copy( x, y, true );
	}
	
	inline bool has_dynamic_extent( const datum_storage& datum )
	{
		const signed char _policy = datum.small[ datum_max_offset ];
		
		return _policy < ~delete_never;
	}
	
	void destroy( datum_storage& datum );
	
	char* reallocate_nothrow( datum_storage& datum, long length );
	char* reallocate        ( datum_storage& datum, long length );
	
	char* extend_capacity_nothrow( datum_storage& datum, long new_capacity );
	char* extend_capacity        ( datum_storage& datum, long new_capacity );
	
	char* curtail_capacity( datum_storage& datum, long new_capacity );
	
	char* set_capacity( datum_storage& datum, long new_capacity );
	
	char* copy_on_write( datum_storage& datum, bool tainting );
	
	unsigned long area( const datum_storage& datum );
	
}

#endif
