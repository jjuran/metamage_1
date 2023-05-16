/*
	functions.cc
	------------
*/

#include "vlib/functions.hh"

// chars
#include "encoding/utf8.hh"

// crypto
#include "md5/md5.hh"
#include "sha1/sha1.hh"
#include "sha256/sha256.hh"

// vxo
#include "vxo/error.hh"

// plus
#include "plus/hexadecimal.hh"
#include "plus/var_string.hh"
#include "plus/string/mince.hh"

// vxo-string
#include "vxo-string/lib/unbin.hh"
#include "vxo-string/lib/unhex.hh"
#include "vxo-string/string.hh"

// bignum
#include "bignum/decode_binoid_int.hh"
#include "bignum/integer_hex.hh"

// vlib
#include "vlib/compare.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	static
	plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	plus::string md5( const plus::string& s )
	{
		crypto::md5_digest hash = crypto::md5( s.data(), s.size() );
		
		return plus::string( (const char*) &hash, sizeof hash );
	}
	
	static
	plus::string sha1( const plus::string& s )
	{
		crypto::sha1_digest hash = crypto::sha1( s.data(), s.size() );
		
		return plus::string( (const char*) &hash, sizeof hash );
	}
	
	static
	plus::string sha256( const plus::string& s )
	{
		crypto::sha256_hash hash = crypto::sha256( s.data(), s.size() );
		
		return plus::string( (const char*) &hash, sizeof hash );
	}
	
	static
	const plus::string& get_string( const vxo::Box& box )
	{
		typedef vxo::Expected< vxo::String > Expected_String;
		
		const Expected_String& xstring = *(const Expected_String*) &box;
		
		if ( const char* error = xstring.error() )
		{
			throw_exception( error );
		}
		
		return xstring.value_cast().get();
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type() )
		{
			default:  THROW( "invalid argument to hex()" );
			
			case Value_number:  return String( hex( arg.number() ) );
			case Value_packed:
			case Value_string:  return String( hex( arg.string() ) );
		}
	}
	
	static
	Value v_half( const Value& arg )
	{
		return Integer( half( arg.number() ) );
	}
	
	static
	Value v_head( const Value& v )
	{
		return first( v );
	}
	
	static
	Value v_areaof( const Value& v )
	{
		return Integer( area( v ) );
	}
	
	static
	Value minmax( const Value& v, bool min_vs_max )
	{
		if ( is_empty_list( v ) )
		{
			return v;
		}
		
		list_iterator args( v );
		
		const Value* result = &args.use();
		
		while ( args )
		{
			const Value& next = args.use();
			
			if ( (compare( *result, next ) > 0) == min_vs_max )
			{
				result = &next;
			}
		}
		
		return *result;
	}
	
	static
	Value v_max( const Value& v )
	{
		return minmax( v, false );
	}
	
	static
	Value v_md5( const Value& v )
	{
		return Packed( md5( v.string() ) );
	}
	
	static
	Value v_min( const Value& v )
	{
		return minmax( v, true );
	}
	
	static
	Value v_mince( const Value& v )
	{
		typedef plus::string::size_type size_t;
		
		list_iterator args( v );
		
		const Value& arg1 = args.use();
		
		const size_t stride = args.get().number().clipped();
		
		if ( stride == 0 )
		{
			THROW( "mince() stride must be positive" );
		}
		
		return VBytes( mince( arg1.string(), stride ),
		               arg1.type(),
		               arg1.dispatch_methods() );
	}
	
	static
	Value v_rep( const Value& v )
	{
		return String( rep( v ) );
	}
	
	static
	Value v_secret( const Value& v )
	{
		return v.secret();
	}
	
	static
	plus::string::size_type substr_offset( const plus::string&     s,
	                                       const bignum::integer&  offset )
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
	plus::string::size_type substr_length( const plus::string&     s,
	                                       const bignum::integer&  length )
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
		
		return String( s.substr( offset, length ) );
	}
	
	static
	Value v_sha1( const Value& v )
	{
		return Packed( sha1( v.string() ) );
	}
	
	static
	Value v_sha256( const Value& v )
	{
		return Packed( sha256( v.string() ) );
	}
	
	static
	Value v_tail( const Value& v )
	{
		return rest( v );
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
				uint8_t c = *p;
				
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
		
		text = String( s );
		
		return text;
	}
	
	static
	Value v_transd( const Value& v )
	{
		list_iterator args( v );
		
		const Value& arg1 = args.use();
		const Value& arg2 = args.use();
		const Value& arg3 = args.get();
		
		plus::var_string s      = arg1.string();
		const plus::string& pat = arg2.string();
		const plus::string& sub = arg3.string();
		
		translate_core( s, pat, sub );
		
		return VBytes( s, arg1.type(), arg1.dispatch_methods() );
	}
	
	static
	bool is_0x_numeral( const plus::string& s, char x )
	{
		return s.size() > 2  &&  s[ 0 ] == '0'  &&  s[ 1 ] == x;
	}
	
	static
	Value v_uchr( const Value& arg )
	{
		using chars::measure_utf8_bytes_for_unicode;
		using chars::put_code_point_into_utf8;
		
		chars::unichar_t uc = arg.number().clipped();
		
		const unsigned n = measure_utf8_bytes_for_unicode( uc );
		
		if ( n == 0 )
		{
			THROW( "invalid Unicode code point" );
		}
		
		plus::string s;
		
		char* p = s.reset( n );
		
		put_code_point_into_utf8( uc, n, p );
		
		return String( s );
	}
	
	static
	Value v_unbin( const Value& v )
	{
		if ( is_0x_numeral( v.string(), 'b' ) )
		{
			return Integer( bignum::unbin_int( v.string().substr( 2 ) ) );
		}
		
		return Packed( get_string( vxo::unbin( v.string() ) ) );
	}
	
	static
	Value v_unhex( const Value& v )
	{
		if ( is_0x_numeral( v.string(), 'x' ) )
		{
			return Integer( bignum::unhex_int( v.string().substr( 2 ) ) );
		}
		
		return Packed( get_string( vxo::unhex( v.string() ) ) );
	}
	
	static const Integer zero = Integer( 0 );
	static const Integer two  = Integer( 2 );
	static const Integer npos = Integer( uint32_t( -1 ) );
	
	static const Type integer = integer_vtype;
	static const Type string  = string_vtype;
	static const Type packed  = packed_vtype;
	
	static const Type i32 = i32_vtype;
	static const Type u32 = u32_vtype;
	
	static const Value bytes( string, Op_union, packed );
	static const Value x32  ( u32,    Op_union, i32    );
	
	static const Value u32_2( u32, Op_duplicate, two );
	static const Value mince( bytes, u32_2 );
	
	static const Value s_offset( x32, Op_duplicate, zero );
	static const Value s_length( u32, Op_duplicate, npos );
	static const Value substr( string, Value( s_offset, s_length ) );
	
	static const Value string_ref( Op_unary_deref, string );
	static const Value trans( string_ref, Value( bytes, bytes ) );
	
	static const Value transd( bytes, Value( bytes, bytes ) );
	
	#define DESTRUCT  "self-destructing"
	#define TRANS     "translate"
	#define TRANSD    "translated"
	
	enum
	{
		pure = Proc_pure,
	};
	
	const proc_info proc_areaof = { "areaof", &v_areaof, NULL,     pure };
	const proc_info proc_half   = { "half",   &v_half,   &integer, pure };
	const proc_info proc_head   = { "head",   &v_head,   NULL,     pure };
	const proc_info proc_hex    = { "hex",    &v_hex,    NULL,     pure };
	const proc_info proc_max    = { "max",    &v_max,    NULL,     pure };
	const proc_info proc_md5    = { "md5",    &v_md5,    &bytes,   pure };
	const proc_info proc_min    = { "min",    &v_min,    NULL,     pure };
	const proc_info proc_mince  = { "mince",  &v_mince,  &mince,   pure };
	const proc_info proc_rep    = { "rep",    &v_rep,    NULL,     pure };
	const proc_info proc_secret = { DESTRUCT, &v_secret, NULL           };
	const proc_info proc_sha1   = { "sha1",   &v_sha1,   &bytes,   pure };
	const proc_info proc_sha256 = { "sha256", &v_sha256, &bytes,   pure };
	const proc_info proc_substr = { "substr", &v_substr, &substr,  pure };
	const proc_info proc_tail   = { "tail",   &v_tail,   NULL,     pure };
	const proc_info proc_trans  = { TRANS,    &v_trans,  &trans         };
	const proc_info proc_transd = { TRANSD,   &v_transd, &transd,  pure };
	const proc_info proc_uchr   = { "uchr",   &v_uchr,   &u32,     pure };
	const proc_info proc_unbin  = { "unbin",  &v_unbin,  &string,  pure };
	const proc_info proc_unhex  = { "unhex",  &v_unhex,  &string,  pure };
	
}
