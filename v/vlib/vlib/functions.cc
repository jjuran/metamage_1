/*
	functions.cc
	------------
*/

#include "vlib/functions.hh"

// crypto
#include "sha256/sha256.hh"

// plus
#include "plus/string/mince.hh"

// plus
#include "plus/binary.hh"
#include "plus/decode_binoid_int.hh"
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"
#include "plus/var_string.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/lib/ed25519.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/vector.hh"


namespace vlib
{
	
	static
	plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	plus::string sha256( const plus::string& s )
	{
		crypto::sha256_hash hash = crypto::sha256( s.data(), s.size() );
		
		return plus::string( (const char*) &hash, sizeof hash );
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type() )
		{
			default:  THROW( "invalid argument to hex()" );
			
			case Value_number:  return hex( arg.number() );
			case Value_vector:
			case Value_string:  return hex( arg.string() );
		}
	}
	
	static
	Value v_abs( const Value& arg )
	{
		return Integer( abs( arg.number() ) );
	}
	
	static
	Value v_half( const Value& arg )
	{
		return Integer( half( arg.number() ) );
	}
	
	static
	Value v_areaof( const Value& v )
	{
		return Integer( area( v ) );
	}
	
	static
	Value v_mince( const Value& v )
	{
		typedef plus::string::size_type size_t;
		
		list_iterator args( v );
		
		const plus::string& string = args.use().string();
		const size_t        stride = args.get().number().clipped();
		
		if ( stride == 0 )
		{
			THROW( "mince() stride must be positive" );
		}
		
		return mince( string, stride );
	}
	
	static
	Value v_rep( const Value& v )
	{
		return rep( v );
	}
	
	static
	plus::string::size_type substr_offset( const plus::string&   s,
	                                       const plus::integer&  offset )
	{
		if ( offset.is_negative() )
		{
			if ( -offset > s.size() )
			{
				THROW( "negative substr offset exceeds string length" );
			}
			
			return s.size() - offset.clipped();
		}
		
		if ( offset > s.size() )
		{
			THROW( "substr offset exceeds string length" );
		}
		
		return offset.clipped();
	}
	
	static inline
	plus::string::size_type substr_length( const plus::string&   s,
	                                       const plus::integer&  length )
	{
		return length.clipped();
	}
	
	static
	Value v_substr( const Value& v )
	{
		list_iterator args( v );
		
		const Value& arg1 = args.use();
		const Value& arg2 = args.use();
		const Value& arg3 = args.get();
		
		typedef plus::string::size_type size_t;
		
		const plus::string& s = arg1.string();
		
		const size_t offset = substr_offset( s, arg2.number() );
		const size_t length = substr_length( s, arg3.number() );
		
		return s.substr( offset, length );
	}
	
	static
	Value v_sha256( const Value& v )
	{
		return Vector( sha256( v.string() ) );
	}
	
	static
	void translate_core( plus::var_string&    s,
	                     const plus::string&  pat,
	                     const plus::string&  sub )
	{
		const char* p_pat = pat.data();
		const char* p_sub = sub.data();
		
		char*   p = s.begin();
		size_t  n = s.size();
		
		size_t n_pat = pat.size();
		size_t n_sub = sub.size();
		
		if ( n_pat != n_sub )
		{
			THROW( "translate()'s find/replace sets must be the same length" );
		}
		
		if ( n_pat > 0 )
		{
			// map
			
			char map[ 256 ];
			
			for ( int i = 0;  i < 256;  ++i )
			{
				map[ i ] = i;
			}
			
			while ( n_pat > 0 )
			{
				unsigned char x = p_pat[ --n_pat ];
				unsigned char c = p_sub[   n_pat ];
				
				map[ x ] = c;
			}
			
			while ( n-- )
			{
				char c = *p;
				
				*p++ = map[ c ];
			}
		}
	}
	
	static
	Value v_trans( const Value& v )
	{
		list_iterator args( v );
		
		const Value& arg1 = args.use();
		const Value& arg2 = args.use();
		const Value& arg3 = args.get();
		
		Target target = make_target( arg1.expr()->right );
		
		Value& text = *target.addr;
		
		// FIXME:  Modify string buffer in-place.
		
		plus::var_string s      = text.string();
		const plus::string& pat = arg2.string();
		const plus::string& sub = arg3.string();
		
		translate_core( s, pat, sub );
		
		text = s;
		
		return text;
	}
	
	static
	bool is_0x_numeral( const plus::string& s, char x )
	{
		return s.size() > 2  &&  s[ 0 ] == '0'  &&  s[ 1 ] == x;
	}
	
	static
	Value v_unbin( const Value& v )
	{
		if ( is_0x_numeral( v.string(), 'b' ) )
		{
			return Integer( unbin_int( v.string().substr( 2 ) ) );
		}
		
		return Vector( unbin( v.string() ) );
	}
	
	static
	Value v_unhex( const Value& v )
	{
		if ( is_0x_numeral( v.string(), 'x' ) )
		{
			return Integer( unhex_int( v.string().substr( 2 ) ) );
		}
		
		return Vector( unhex( v.string() ) );
	}
	
	static
	void check_ed25519_key_size( const plus::string& key )
	{
		if ( key.size() != 32 )
		{
			THROW( "Ed25519 keys must be 32 bytes" );
		}
	}
	
	static
	void check_ed25519_sig_size( const plus::string& sig )
	{
		if ( sig.size() != 64 )
		{
			THROW( "Ed25519 signatures must be 64 bytes" );
		}
	}
	
	static
	Value v_mkpub( const Value& v )
	{
		const plus::string& secret_key = v.string();
		
		check_ed25519_key_size( secret_key );
		
		return Vector( ed25519::publickey( secret_key ) );
	}
	
	static
	Value v_sign( const Value& v )
	{
		list_iterator args( v );
		
		const plus::string& key = args.use().string();
		const plus::string& msg = args.get().string();
		
		check_ed25519_key_size( key );
		
		return Vector( ed25519::sign( key, msg ) );
	}
	
	static
	Value v_verify( const Value& v )
	{
		list_iterator args( v );
		
		const plus::string& key = args.use().string();
		const plus::string& msg = args.use().string();
		const plus::string& sig = args.get().string();
		
		check_ed25519_key_size( key );
		check_ed25519_sig_size( sig );
		
		bool b = ed25519::verify( key, msg, sig );
		
		return Boolean( b );
	}
	
	static const Integer zero = Integer( 0 );
	static const Integer two  = Integer( 2 );
	static const Integer npos = Integer( uint32_t( -1 ) );
	
	static const Value integer = integer_vtype;
	static const Value string  = string_vtype;
	static const Value vector  = vector_vtype;
	
	static const Value u32_2 = Value( u32_vtype, Op_duplicate, two );
	static const Value mince = Value( string, u32_2 );
	
	static const Value bytes( string_vtype, Op_union, vector_vtype );
	static const Value sign( vector_vtype, bytes );
	static const Value verify( vector_vtype, Value( bytes, vector_vtype ) );
	static const Value x32( u32_vtype, Op_union, i32_vtype );
	static const Value s_offset( x32, Op_duplicate, zero );
	static const Value s_length( u32_vtype, Op_duplicate, npos );
	static const Value substr( string_vtype, Value( s_offset, s_length ) );
	
	static const Value string_ref = Value( Op_unary_deref, string_vtype );
	static const Value trans( string_ref, Value( string_vtype, string_vtype ) );
	
	#define TRANS  "translate"
	
	const proc_info proc_abs    = { "abs",    &v_abs,    &integer };
	const proc_info proc_areaof = { "areaof", &v_areaof, NULL     };
	const proc_info proc_half   = { "half",   &v_half,   &integer };
	const proc_info proc_hex    = { "hex",    &v_hex,    NULL     };
	const proc_info proc_mince  = { "mince",  &v_mince,  &mince   };
	const proc_info proc_rep    = { "rep",    &v_rep,    NULL     };
	const proc_info proc_sha256 = { "sha256", &v_sha256, &bytes   };
	const proc_info proc_substr = { "substr", &v_substr, &substr  };
	const proc_info proc_trans  = { TRANS,    &v_trans,  &trans   };
	const proc_info proc_unbin  = { "unbin",  &v_unbin,  &string  };
	const proc_info proc_unhex  = { "unhex",  &v_unhex,  &string  };
	
	const proc_info proc_mkpub  = { "ed25519-publickey", &v_mkpub,  &vector };
	const proc_info proc_sign   = { "ed25519-sign",      &v_sign,   &sign   };
	const proc_info proc_verify = { "ed25519-verify",    &v_verify, &verify };
	
}
