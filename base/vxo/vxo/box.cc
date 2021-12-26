/*
	box.cc
	------
*/

#include "vxo/box.hh"

// iota
#include "iota/swap.hh"

// vxo
#include "vxo/extent.hh"


#pragma exceptions off


namespace vxo
{

void Box::unshare_extent()
{
	if ( extent_refcount( u.str.pointer ) > 1 )
	{
		u.str.pointer = extent_unshare( (char*) u.str.pointer );
		
		if ( subtype_byte() >= Box_container )
		{
			Box* it = (Box*) u.str.pointer;
			
			long n = u.str.length;
			
			while ( n-- > 0 )
			{
				Box& box = *it++;
				
				if ( box.has_extent() )
				{
					extent_add_ref( box.u.str.pointer );
				}
			}
		}
	}
}

void Box::destroy_extent()
{
	if ( subtype_byte() >= Box_container )
	{
		if ( extent_refcount( u.str.pointer ) == 1 )
		{
			Box* it = (Box*) u.str.pointer;
			
			long n = u.str.length;
			
			while ( n-- > 0 )
			{
				it++->destroy();
			}
		}
	}
	
	extent_release( u.str.pointer );
	
	set_control_byte( 0 );
}

Box::Box( const Box& that )
{
	u = that.u;
	
	if ( has_extent() )
	{
		extent_add_ref( u.str.pointer );
	}
}

Box& Box::operator=( const Box& that )
{
	if ( that.has_extent() )
	{
		extent_add_ref( that.u.str.pointer );
	}
	
	if ( has_extent() )
	{
		extent_release( u.str.pointer );
	}
	
	u = that.u;
	
	return *this;
}

void Box::swap( Box& v )
{
	using iota::swap;
	
	swap( u, v.u );
}

}
