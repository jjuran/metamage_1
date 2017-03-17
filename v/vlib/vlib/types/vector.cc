/*
	vector.cc
	---------
*/

#include "vlib/types/vector.hh"

// gear
#include "gear/hexadecimal.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	Value Vector::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_vector:
				return v;
			
			default:
				THROW( "type not convertible to vector" );
			
			case Value_empty_list:
				return Vector();
			
			case Value_string:
				return Vector( v.string() );
			
			case Value_byte:
			case Value_mb32:
			case Value_pair:
				return Vector( pack( v ) );
		}
	}
	
	static
	size_t vector_str_size( const Value& v )
	{
		return v.string().size() * 2;
	}
	
	static
	char* vector_str_copy( char* p, const Value& v )
	{
		const plus::string& s = v.string();
		
		return gear::hexpcpy_lower( p, s.data(), s.size() );
	}
	
	static const stringify vector_str =
	{
		NULL,
		&vector_str_size,
		&vector_str_copy,
	};
	
	static
	size_t vector_rep_size( const Value& v )
	{
		return vector_str_size( v ) + 3;
	}
	
	static
	char* vector_rep_copy( char* p, const Value& v )
	{
		*p++ = 'x';
		*p++ = '"';
		
		p = vector_str_copy( p, v );
		
		*p++ = '"';
		
		return p;
	}
	
	static const stringify vector_rep =
	{
		NULL,
		&vector_rep_size,
		&vector_rep_copy,
	};
	
	static
	const char* vector_bin_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t vector_bin_size( const Value& v )
	{
		return v.string().size();
	}
	
	static const stringify vector_bin =
	{
		&vector_bin_data,
		&vector_bin_size,
		NULL,
	};
	
	static const stringifiers vector_stringifiers =
	{
		&vector_str,
		&vector_rep,
		&vector_bin,
	};
	
	const dispatch vector_dispatch =
	{
		&vector_stringifiers,
	};
	
	static
	Value vector_member( const Value&         obj,
	                     const plus::string&  member )
	{
		if ( member == "size" )
		{
			return Integer( obj.string().size() );
		}
		
		if ( member == "string" )
		{
			return String( obj.string() );
		}
		
		THROW( "nonexistent vector member" );
		
		return Value_nothing;
	}
	
	const type_info vector_vtype =
	{
		"vector",
		&assign_to< Vector >,
		&Vector::coerce,
		&vector_member,
	};
	
}
