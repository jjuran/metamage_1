/*
	byte.cc
	-------
*/

#include "vlib/types/endec/byte.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	Value v_encode( const Value& v )
	{
		return Packed( plus::string( 1, v.as< Byte >().get() ) );
	}
	
	static
	const char* get_pack_data( const Value& v )
	{
		enum { size = 1 };
		
		const plus::string& pack = v.string();
		
		if ( pack.size() != size )
		{
			THROW( "decode arg has wrong size" );
		}
		
		return pack.data();
	}
	
	static
	Value v_decode( const Value& v )
	{
		return Byte( *get_pack_data( v ) );
	}
	
	static const Type byte   = byte_vtype;
	static const Type packed = packed_vtype;
	
	enum
	{
		pure = Proc_pure,
	};
	
	const proc_info proc_encode_byte = { "encode", &v_encode, &byte,   pure };
	const proc_info proc_decode_byte = { "decode", &v_decode, &packed, pure };
	
}
