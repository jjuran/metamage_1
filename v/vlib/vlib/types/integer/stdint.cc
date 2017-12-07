/*
	stdint.cc
	---------
*/

#include "vlib/types/integer/stdint.hh"

// Standard C
#include <string.h>

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/type.hh"
#include "vlib/types/endec/stdint.hh"


#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace vlib
{
	
	class StdInt : public Value
	{
		public:
			StdInt( const type_info&  type,
			        const proc_info&  encode,
			        const proc_info&  decode );
			
			const type_info& base_type() const
			{
				return this->expr()->left.expr()->left.typeinfo();
			}
			
			const char* name() const
			{
				return base_type().name;
			}
			
			const Value& size() const
			{
				return this->expr()->left.expr()->right;
			}
			
			const Value& encoder() const
			{
				return this->expr()->right.expr()->left;
			}
			
			const Value& decoder() const
			{
				return this->expr()->right.expr()->right;
			}
	};
	
	static
	const char* stdint_str_data( const Value& v )
	{
		const StdInt& type = (const StdInt&) v;
		
		return type.name();
	}
	
	static const stringify stdint_str =
	{
		&stdint_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers stdint_stringifiers =
	{
		&stdint_str,
		// For rep, fall back to str
		// Type bin is unimplemented
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( type_vtype );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	const Value* encode_decode( const StdInt& type, const plus::string& member )
	{
		const char*  member_data = member.data();
		const size_t member_size = member.size();
		
		if ( member_size == STRLEN( "xxcode-native" ) )
		{
			if ( memcmp( member_data + 2, STR_LEN( "code-native" ) ) != 0 )
			{
				return NULL;
			}
		}
		else if ( member_size == STRLEN( "xxcode" ) )
		{
			if ( type.size().number().clipped() != 1 )
			{
				// Only byte-sized ints have unambiguous coding.
				return NULL;
			}
			
			if ( memcmp( member_data + 2, STR_LEN( "code" ) ) != 0 )
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
		
		if ( memcmp( member_data, STR_LEN( "en" ) ) == 0 )
		{
			return &type.encoder();
		}
		
		if ( memcmp( member_data, STR_LEN( "de" ) ) == 0 )
		{
			return &type.decoder();
		}
		
		return NULL;
	}
	
	static
	Value stdint_member( const StdInt& type, const plus::string& name )
	{
		if ( name == "size" )
		{
			return type.size();
		}
		
		if ( const Value* endec = encode_decode( type, name ) )
		{
			return *endec;
		}
		
		THROW( "no such stdint member" );
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const StdInt& type = (const StdInt&) a;
		
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return type.base_type().coerce( b );
			
			case Op_subscript:
				if ( is_empty_list( b ) )
				{
					return Value( a, op, b );
				}
			
			case Op_member:
				if ( b.type() != V_str )
				{
					THROW( "non-string member name" );
				}
				
				return stdint_member( type, b.string() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	static
	bool typecheck( const Value& t, const Value& v )
	{
		const StdInt& type = (const StdInt&) t;
		
		return type.base_type().assign( v ).type();
	}
	
	static
	Value transform( const Value& t, const Value& v )
	{
		const StdInt& type = (const StdInt&) t;
		
		return type.base_type().coerce( v );
	}
	
	static const typing type =
	{
		&typecheck,
		&transform,
	};
	
	static const dispatch stdint_dispatch =
	{
		&stdint_stringifiers,
		NULL,
		NULL,
		&ops,
		&type,
	};
	
	static
	Value type_size( const type_info& type )
	{
		const char* name = type.name;
		
		++name;
		
		uint8_t a = *name++;
		uint8_t b = *name;
		
		size_t size = 1;  // if either i8 or u8
		
		if ( b != 0 )
		{
			a -= '0';
			b -= '0';
			
			size = (a * 10 + b) / 8;
		}
		
		return Value( Type( type ), Op_mapping, Integer( size ) );
	}
	
	StdInt::StdInt( const type_info&  type,
	                const proc_info&  encode,
	                const proc_info&  decode )
	:
		Value( type_size( type ),
		       Op_typeof,
		       Value( Proc( encode ), Proc( decode ) ),
		       &stdint_dispatch )
	{
	}
	
	Value i8_type()
	{
		return StdInt( i8_vtype, proc_encode_i8, proc_decode_i8 );
	}
	
	Value u8_type()
	{
		return StdInt( u8_vtype, proc_encode_u8, proc_decode_u8 );
	}
	
	#define DEFINE_MULTIBYTE_STDINT( type )  \
	Value type##_type()  \
	{                    \
		return StdInt( type##_vtype,                 \
		               proc_encode_native_##type,    \
		               proc_decode_native_##type );  \
	}
	
	DEFINE_MULTIBYTE_STDINT( i16 )
	DEFINE_MULTIBYTE_STDINT( i32 )
	DEFINE_MULTIBYTE_STDINT( i64 )
	DEFINE_MULTIBYTE_STDINT( u16 )
	DEFINE_MULTIBYTE_STDINT( u32 )
	DEFINE_MULTIBYTE_STDINT( u64 )
	
}
