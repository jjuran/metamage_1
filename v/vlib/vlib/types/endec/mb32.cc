/*
	mb32.cc
	-------
*/

#include "vlib/types/endec/mb32.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/types/field.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"


namespace vlib
{

const int mb32_size = 4;

static
Value v_encode( const Value& v )
{
	uint32_t data = v.as< MB32 >().get();
	
	return Packed( plus::string( (char*) &data, mb32_size ) );
}

static
uint32_t get_pack_data( const Value& v )
{
	const plus::string& pack = v.string();
	
	if ( pack.size() != mb32_size )
	{
		THROW( "decode arg has wrong size" );
	}
	
	return *(const uint32_t*) pack.data();
}

static
Value v_decode( const Value& v )
{
	return MB32( get_pack_data( v ) );
}

static const Type mb32   = mb32_vtype;
static const Type packed = packed_vtype;

enum
{
	pure = Proc_pure,
};

static const proc_info encode_mb32 = { "encode", &v_encode, &mb32,   pure };
static const proc_info decode_mb32 = { "decode", &v_decode, &packed, pure };

Value mb32_type()
{
	return Field( mb32_size, mb32_vtype, encode_mb32, decode_mb32 );
}

}
