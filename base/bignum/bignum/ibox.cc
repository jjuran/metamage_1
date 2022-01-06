/*
	ibox.cc
	-------
*/

#include "bignum/ibox.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/endian.hh"

// math
#include "math/integer.hh"

// debug
#include "debug/assert.hh"

// vxo
#include "vxo/extent.hh"


namespace bignum
{
	
	using math::integer::limb_t;
	using math::integer::cmp_t;
	
	using vxo::extent_release;
	using vxo::extent_unshare;
	using vxo::extent_alloc;
	using vxo::extent_add_ref;
	using vxo::extent_area;
	
	
	void ibox::destroy_extent()
	{
		extent_release( (char*) its.pointer );
	}
	
	void ibox::unshare()
	{
		if ( has_extent() )
		{
			its.pointer = (ptr_t) extent_unshare( (char*) its.pointer );
		}
	}
	
	void ibox::construct( long long i )
	{
		sign_t sign = Sign_positive;
		
		if ( i < 0 )
		{
			const int_t      max_limb = int_t( -1 );
			const long long  min_limb = -1LL * max_limb;
			
			if ( i >= min_limb )
			{
				construct( (unsigned long) -i );
				
				its.sign = Sign_negative;
				
				return;
			}
			
			// 0x8000 0000 0000 0000
			const long long nadir = 1LL << (sizeof (long long) * 8 - 1);
			
			if ( i != nadir )
			{
				i = -i;
			}
			
			sign = Sign_negative;
		}
		
		const unsigned n_limbs = sizeof (long long) / sizeof (int_t);
		
		construct( (int_t const*) &i, n_limbs, sign );
	}
	
	void ibox::construct( int_t const*  data,
	                      size_t        n,
	                      sign_t        sign )
	{
		ASSERT( n > 1 );
		
		char* extent = extent_alloc( n * sizeof (int_t) );
		
		// Copy before clobbering the struct
		// This is strictly a copy, not a move, but memmove() is smaller.
		
		memmove( extent, data, n * sizeof (int_t) );
		
		its.pointer = (ptr_t) extent;
		its.size    = n;
		its.sign    = sign;
	}
	
	ibox::ibox( const ibox& that )
	{
		(ibox_structure&) *this = (const ibox_structure&) that;
		
		if ( that.has_extent() )
		{
			extent_add_ref( (char*) that.its.pointer );
		}
	}
	
	ibox& ibox::operator=( const ibox& that )
	{
		if ( this != &that )
		{
			destroy();
			
			(ibox_structure&) *this = (const ibox_structure&) that;
			
			if ( that.has_extent() )
			{
				vxo::extent_add_ref( (char*) that.its.pointer );
			}
		}
		
		return *this;
	}
	
	void ibox::swap( ibox& i )
	{
		ibox_structure tmp = (ibox_structure&) *this;
		
		(ibox_structure&) *this = (ibox_structure&) i;
		
		(ibox_structure&) i = tmp;
	}
	
	limb_t ibox::extent_top() const
	{
		ASSERT( has_extent() );
		
		const bool be = ! iota::is_little_endian();
		
		limb_t const* biggest = be ? its.pointer
		                           : its.pointer + its.size - 1;
		
		return *biggest;
	}
	
	limb_t ibox::extent_bottom() const
	{
		ASSERT( has_extent() );
		
		const bool le = iota::is_little_endian();
		
		limb_t const* littlest = le ? its.pointer
		                            : its.pointer + its.size - 1;
		
		return *littlest;
	}
	
	unsigned long long ibox::long_bottom() const
	{
		if ( ! has_extent() )
		{
			return its.integer;
		}
		
		ASSERT( sizeof (int_t) * 2 >= sizeof (unsigned long long) );
		
		unsigned long long result = 0;
		
		const bool le = iota::is_little_endian();
		
		limb_t const* littlest = le ? its.pointer
		                            : its.pointer + its.size - 2;
		
		return *(const unsigned long long*) littlest;
	}
	
	void ibox::extend( size_t n )
	{
		if ( n < its.size )
		{
			throw limb_count_overflow();
		}
		
		ASSERT( n > 1        );
		ASSERT( n > its.size );
		
		int_t const* old_data = data();
		const size_t old_size = size();
		
		const size_t n_zeros = n - old_size;
		
		int_t* const new_data = (ptr_t) extent_alloc( n * sizeof (int_t) );
		
		int_t* p = new_data;
		
		if ( iota::is_little_endian() )
		{
			p = (int_t*) mempcpy( p, old_data, old_size * sizeof (int_t) );
			
			memset( p, '\0', n_zeros * sizeof (int_t) );
		}
		else
		{
			memset( p, '\0', n_zeros * sizeof (int_t) );
			
			p += n_zeros;
			
			// This is strictly a copy, not a move, but memmove() is smaller.
			
			memmove( p, old_data, old_size * sizeof (int_t) );
		}
		
		destroy();
		
		its.pointer = new_data;
		its.size    = n;
	}
	
	void ibox::shrink_to_fit()
	{
		ASSERT( its.size > 1 );
		
		if ( iota::is_little_endian() )
		{
			size_t size = its.size;
			
			while ( its.pointer[ size - 1 ] == 0 )
			{
				--size;
				
				ASSERT( size != 0 );
			}
			
			its.size = size;
		}
		else
		{
			size_t n_zeros  = 0;
			
			while ( its.pointer[ n_zeros ] == 0 )
			{
				++n_zeros;
				
				ASSERT( n_zeros != its.size );
			}
			
			if ( n_zeros != 0 )
			{
				its.size -= n_zeros;
				
				memmove( its.pointer,
				         its.pointer + n_zeros,
				         its.size * sizeof (int_t) );
			}
		}
		
		if ( its.size == 1 )
		{
			const int_t x = *its.pointer;
			
			ASSERT( x != 0 );
			
			destroy_extent();
			
			its.size = 0;
			its.integer = x;
		}
	}
	
	
	cmp_t abs_compare( const ibox& a, const ibox& b )
	{
		using math::integer::compare;
		
		return compare( iota::is_little_endian(), a.data(), a.size(),
		                                          b.data(), b.size() );
	}
	
	cmp_t compare( const ibox& a, const ibox& b )
	{
		/*
			Subtract one sign from the other.  The result is in range (-2 .. 2)
			but we need it mapped to [-1, -1, 0, 1, 1].
			
			For non-zero sign_cmp:
			
			0010 >> (0010 >> 1 & 1) -> 0010 >> (0001 & 1) -> 0010 >> 1 -> 0001
			0001 >> (0001 >> 1 & 1) -> 0001 >> (0000 & 1) -> 0001 >> 0 -> 0001
			1111 >> (1111 >> 1 & 1) -> 1111 >> (1111 & 1) -> 1111 >> 1 -> 1111
			1110 >> (1110 >> 1 & 1) -> 1110 >> (1111 & 1) -> 1110 >> 1 -> 1111
		*/
		
		if ( cmp_t sign_cmp = a.sign() - b.sign() )
		{
			return sign_cmp >> ((sign_cmp >> 1) & 1);
		}
		
		if ( a.sign() == 0 )
		{
			return 0;
		}
		
		cmp_t abs_cmp = abs_compare( a, b );
		
		if ( a.sign() < 0 )
		{
			abs_cmp = -abs_cmp;
		}
		
		return abs_cmp;
	}
	
	
	static inline
	bool sum_is_one_limb( ibox::size_t  a_size,
	                      ibox::size_t  b_size,
	                      limb_t        a,
	                      limb_t        b )
	{
		using math::integer::will_carry;
		
		// These are the raw size fields, not size().
		
		return a_size == 0  &&  b_size == 0  &&  ! will_carry( a, b );
	}
	
	static inline
	limb_t one_limb_product( ibox::size_t  a_size,
	                         ibox::size_t  b_size,
	                         limb_t        a,
	                         limb_t        b )
	{
		/*
			Return the product a * b if it fits within a single limb.
			Return zero otherwise.
			
			Since the result is ambiguous when either operand is zero,
			callers must check for zero operands in advance and not call
			this function in that case.
		*/
		
		// These are the raw size fields, not size().
		
		if ( a_size != 0  ||  b_size != 0 )
		{
			return 0;
		}
		
		using math::integer::long_t;
		using math::integer::long_multiply;
		
		if ( sizeof (long_t) > sizeof (limb_t) )
		{
			const long_t product = long_multiply( a, b );
			
			return (limb_t) product == product ? product : 0;
		}
		
		/*
			In the absence of a means to detect multiplication overflow,
			we examine the width of the operands and guess conservatively.
			If this function returns nonzero, then the product of the operands
			will fit in a single limb.  The converse doesn't hold, however.
			
			zenith:  The maximum int_t value (unsigned).
			factor:  The maximum squarable factor.
			
			E.g. For 64-bit systems, zenith is 0xFFFFFFFFFFFFFFFF and factor
			is 0xFFFFFFFF.  (32-bit systems are handled above.)
		*/
		
		const ibox::int_t zenith = ibox::int_t( -1 );
		const ibox::int_t factor = zenith >> (sizeof zenith * 8 / 2);
		
		return (a <= factor  &&  b <= factor) ? a * b : 0;
	}
	
	
	void ibox::add( const ibox& y )
	{
		const bool single = sum_is_one_limb( its.size,    y.its.size,
		                                     its.integer, y.its.integer );
		
		if ( single )
		{
			its.integer += y.its.integer;
			
			return;
		}
		
		using math::integer::sum_size;
		using math::integer::add;
		
		ibox tmp = y;  // in case this == &y
		
		limb_t const* x_data = data();
		size_t        x_size = size();
		limb_t const* y_data = tmp.data();
		size_t        y_size = tmp.size();
		
		ASSERT( x_size != 0 );
		ASSERT( y_size != 0 );
		
		const size_t r_size = sum_size( iota::is_little_endian(),
		                                x_data, x_size,
		                                y_data, y_size );
		
		ASSERT( r_size > 1 );
		
		if ( r_size > x_size )
		{
			extend( r_size );
			
			x_size = r_size;
		}
		else
		{
			unshare();
		}
		
		add( iota::is_little_endian(), its.pointer, x_size, y_data, y_size );
	}
	
	void ibox::subtract_lesser( const ibox& y )
	{
		ASSERT( abs_compare( y, *this ) < 0 );
		
		if ( ! has_extent() )
		{
			its.integer -= y.its.integer;
			
			return;
		}
		
		using math::integer::subtract;
		
		ibox tmp = y;  // in case this == &y
		
		unshare();
		
		limb_t*       x_data = its.pointer;
		size_t        x_size = size();
		limb_t const* y_data = tmp.data();
		size_t        y_size = tmp.size();
		
		subtract( iota::is_little_endian(), x_data, x_size, y_data, y_size );
		
		shrink_to_fit();
	}
	
	void ibox::subtract( const ibox& y )
	{
		const cmp_t rel = abs_compare( *this, y );
		
		if ( rel == 0 )
		{
			destroy();
			construct( 0ul );
			
			return;
		}
		
		if ( rel > 0 )
		{
			subtract_lesser( y );
		}
		else
		{
			ibox result = y;
			
			result.subtract_lesser( *this );
			
			swap( result );
		}
	}
	
	void ibox::multiply_by( const ibox& y )
	{
		ASSERT(   its.sign != 0 );
		ASSERT( y.its.sign != 0 );
		
		const limb_t product = one_limb_product( its.size,    y.its.size,
		                                         its.integer, y.its.integer );
		
		if ( product != 0 )
		{
			its.integer = product;
		}
		else
		{
			using math::integer::multiply;
			
			ibox tmp = y;  // in case this == &y
			
			size_t x_size = size();
			size_t y_size = y.size();
			
			x_size += y_size;
			
			extend( x_size );
			
			limb_t*       x_data = its.pointer;
			limb_t const* y_data = tmp.data();
			
			multiply( iota::is_little_endian(),
			          x_data, x_size,
			          y_data, y_size );
			
			shrink_to_fit();
		}
		
		its.sign *= y.its.sign;
	}
	
	void ibox::halve()
	{
		if ( has_extent() )
		{
			using math::integer::shift_right;
			
			unshare();
			
			shift_right( iota::is_little_endian(), its.pointer, size() );
			
			shrink_to_fit();
		}
		else
		{
			its.integer /= 2;
			
			if ( its.integer == 0 )
			{
				its.sign = 0;
			}
		}
		
	}
	
	unsigned long ibox::area() const
	{
		if ( ! has_extent() )
		{
			return sizeof (ibox);
		}
		
		return sizeof (ibox) + extent_area( (const char*) its.pointer );
	}
	
}
