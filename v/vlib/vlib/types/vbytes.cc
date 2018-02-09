/*
	vbytes.cc
	---------
*/

#include "vlib/types/vbytes.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/byte.hh"


namespace vlib
{
	
	static
	const char* vbytes_cpy_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t vbytes_cpy_size( const Value& v )
	{
		return v.string().size();
	}
	
	const stringify vbytes_cpy =
	{
		&vbytes_cpy_data,
		&vbytes_cpy_size,
		NULL,
	};
	
	
	Value assign_byte_to_index( Value& s, const Value& x, const Byte& byte )
	{
		Expr* expr = x.expr();
		
		const Value& subscript = expr->right;
		
		const unsigned i = subscript_integer( subscript );
		
		if ( i >= s.string().size() )
		{
			THROW( "subscript exceeds string/pack bounds" );
		}
		
		char* data = (char*) s.string().data();
		
		data[ i ] = byte;
		
		return byte;
	}
	
}
