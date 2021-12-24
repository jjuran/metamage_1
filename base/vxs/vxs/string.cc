/*
	string.cc
	---------
*/

#include "vxs/string.hh"

// plus
#include "plus/extent.hh"


#pragma exceptions off


namespace vxo
{

DynamicString::DynamicString( const plus::string& s )
{
	u.str = *(u_string*) &s;
	
	if ( has_extent() )
	{
		plus::extent_add_ref( u.str.pointer );
	}
	
	if ( has_pointer() )
	{
		set_subtype_byte( Box_string );
	}
}

DynamicString& DynamicString::operator=( const string& s )
{
	if ( (const void*) &s != (void*) this )
	{
		if ( has_extent() )
		{
			plus::extent_release( u.str.pointer );
		}
		
		u.str = *(u_string*) &s;
		
		if ( has_extent() )
		{
			plus::extent_add_ref( u.str.pointer );
		}
		
		if ( has_pointer() )
		{
			set_subtype_byte( Box_string );
		}
	}
	
	return *this;
}

}
