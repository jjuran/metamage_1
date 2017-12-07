/*
	stdint.cc
	---------
*/

#include "vlib/types/endec/stdint.hh"

// Standard C
#include <stddef.h>

// vlib
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	Value pack( const void* data, size_t size )
	{
		return Packed( plus::string( (const char*) data, size ) );
	}
	
	template < class Int >
	static
	Value v_encode( const Value& v )
	{
		const Int x = integer_cast< Int >( v );
		
		return pack( &x, sizeof x );
	}
	
	static
	const void* get_pack_data( const Value& v, size_t size )
	{
		const plus::string& pack = v.string();
		
		if ( pack.size() != size )
		{
			THROW( "decode arg has wrong size" );
		}
		
		return pack.data();
	}
	
	template < class Int >
	static
	Value v_decode( const Value& v )
	{
		const Int& x = *(const Int*) get_pack_data( v, sizeof (Int) );
		
		return Integer( x );
	}
	
	static const Type integer = integer_vtype;
	static const Type packed  = packed_vtype;
	
	#define ENCODE( I )  { "encode", &v_encode< I >, &integer }
	#define DECODE( I )  { "decode", &v_decode< I >, &packed  }
	
	#define ENCODE_NATIVE( I )  { "encode-native", &v_encode< I >, &integer }
	#define DECODE_NATIVE( I )  { "decode-native", &v_decode< I >, &packed  }
	
	const proc_info proc_encode_i8 = ENCODE( int8_t );
	const proc_info proc_decode_i8 = DECODE( int8_t );
	
	const proc_info proc_encode_native_i16 = ENCODE_NATIVE( int16_t );
	const proc_info proc_decode_native_i16 = DECODE_NATIVE( int16_t );
	
	const proc_info proc_encode_native_i32 = ENCODE_NATIVE( int32_t );
	const proc_info proc_decode_native_i32 = DECODE_NATIVE( int32_t );
	
	const proc_info proc_encode_native_i64 = ENCODE_NATIVE( int64_t );
	const proc_info proc_decode_native_i64 = DECODE_NATIVE( int64_t );
	
	const proc_info proc_encode_u8 = ENCODE( uint8_t );
	const proc_info proc_decode_u8 = DECODE( uint8_t );
	
	const proc_info proc_encode_native_u16 = ENCODE_NATIVE( uint16_t );
	const proc_info proc_decode_native_u16 = DECODE_NATIVE( uint16_t );
	
	const proc_info proc_encode_native_u32 = ENCODE_NATIVE( uint32_t );
	const proc_info proc_decode_native_u32 = DECODE_NATIVE( uint32_t );
	
	const proc_info proc_encode_native_u64 = ENCODE_NATIVE( uint64_t );
	const proc_info proc_decode_native_u64 = DECODE_NATIVE( uint64_t );
	
}
