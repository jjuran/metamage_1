/*
	vbytes.cc
	---------
*/

#include "vlib/types/vbytes.hh"

// gear
#include "gear/find.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/byte.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/integer.hh"


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
	
	
	static
	Value division( const VBytes& bytes, bignum::integer x )
	{
		typedef bignum::ibox::sign_t     sign_t;
		typedef plus::string::size_type  size_t;
		
		const sign_t sign = x.sign();
		
		if ( sign == 0 )
		{
			THROW( "division by zero (of packed data)" );
		}
		
		x.absolve();  // in-place absolute value
		
		if ( ! x.demotes_to< size_t >() )
		{
			THROW( "divisor of packed data is excessively large" );
		}
		
		const plus::string& s = bytes.string();
		
		const size_t divisor = x.clipped_to< size_t >();
		const size_t n_bytes = s.size();
		
		const size_t quotient  = n_bytes / divisor;
		const size_t remainder = n_bytes % divisor;
		
		if ( remainder != 0 )
		{
			THROW( "division of packed data with non-zero remainder" );
		}
		
		/*
			Let S be the size of the input data in bytes.
			Let N be the divisor argument.
			Let Q be S/abs(N).
			
			Dividing by N > 0 yields N pieces of length Q.
			Dividing by N < 0 yields Q pieces of length -N.
		*/
		
		const size_t chunk_len = sign > 0 ? quotient
		                                  : divisor;
		
		size_t n_chunks = sign > 0 ? divisor
		                           : quotient;
		
		list_builder result;
		
		size_t i = 0;
		
		while ( n_chunks-- > 0 )
		{
			result.append( VBytes( s.substr( i, chunk_len ),
			                       bytes.type(),
			                       bytes.dispatch_methods() ) );
			
			i += chunk_len;
		}
		
		return result.get();
	}
	
	static
	Value division( const VBytes& bytes, char c )
	{
		typedef plus::string::size_type  size_t;
		
		const plus::string& s = bytes.string();
		
		const char* begin = s.data();
		const size_t size = s.size();
		
		const char* end = begin + size;
		
		const char* q = gear::find_first_match( begin, end, c, NULL );
		
		if ( q == NULL )
		{
			return bytes;
		}
		
		const char* p = begin;
		
		list_builder result;
		
		goto start;
		
		do
		{
			p = ++q;
			
			q = gear::find_first_match( q, end, c, end );
			
		start:
			
			result.append( VBytes( s.substr( p - begin, q - p ),
			                       bytes.type(),
			                       bytes.dispatch_methods() ) );
		}
		while ( q < end );
		
		return result.get();
	}
	
	Value division( const VBytes& bytes, const Value& divisor )
	{
		if ( const Integer* integer = divisor.is< Integer >() )
		{
			return division( bytes, integer->get() );
		}
		
		if ( const Byte* byte = divisor.is< Byte >() )
		{
			return division( bytes, byte->get() );
		}
		
		THROW( "divisor of bytes must be an integer or byte" );
		
		return Value();
	}
	
}
