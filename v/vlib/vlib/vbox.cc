/*
	vbox.cc
	-------
*/

#include "vlib/vbox.hh"

// iota
#include "iota/swap.hh"

// plus
#include "plus/extent.hh"


namespace vlib
{
	
	const int vbox_inline_size = sizeof (vu_string) - 1;
	
	void vbox::destroy_extent()
	{
		plus::extent_release( u.alloc.pointer );
		
		u.alloc.type = 0;
	}
	
	void vbox::unshare()
	{
		if ( has_extent() )
		{
			u.alloc.pointer = plus::extent_unshare( (char*) u.alloc.pointer );
		}
	}
	
	vbox::vbox( const vbox& that )
	{
		u = that.u;
		
		if ( has_extent() )
		{
			plus::extent_add_ref( u.alloc.pointer );
		}
	}
	
	vbox& vbox::operator=( const vbox& that )
	{
		if ( that.has_extent() )
		{
			plus::extent_add_ref( that.u.alloc.pointer );
		}
		
		if ( has_extent() )
		{
			plus::extent_release( u.alloc.pointer );
		}
		
		u = that.u;
		
		return *this;
	}
	
	vbox::vbox( const vu_string& s, char semantics )
	{
		(vu_string&) *this = s;
		
		if ( u.chars.data[ vbox_inline_size ] < 0 )
		{
			plus::extent_add_ref( u.alloc.pointer );
			
			u.alloc.type = Vbox_alloc;
		}
		else
		{
			u.alloc.type = Vbox_string;
		}
		
		u.alloc.semantics = semantics;
	}
	
	vbox::vbox( const vu_ibox& ix, char semantics )
	{
		(vu_ibox&) *this = ix;
		
		if ( u.ibox.size > 0 )
		{
			plus::extent_add_ref( u.alloc.pointer );
			
			u.alloc.type = Vbox_alloc;
		}
		else
		{
			u.alloc.type = Vbox_bigint;
		}
		
		u.alloc.semantics = semantics;
	}
	
	vbox::vbox( unsigned long n, destructor dtor, char semantics )
	{
		char* extent = plus::extent_alloc( n, dtor );
		
		u.alloc.pointer   = extent;
		u.alloc.type      = Vbox_alloc;
		u.alloc.semantics = semantics;
	}
	
	void vbox::swap( vbox& v )
	{
		using iota::swap;
		
		swap( u, v.u );
	}
	
	unsigned long area( const vbox& box )
	{
		if ( box.has_extent() )
		{
			return sizeof (vbox) + plus::extent_area( box.u.alloc.pointer );
		}
		
		return sizeof (vbox);
	}
	
}
