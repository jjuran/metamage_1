/*
	math/integer.hh
	---------------
*/

#ifndef MATH_INTEGER_HH
#define MATH_INTEGER_HH

// math
#include "math/integer_types.hh"

/*
	Metamage Integer Numerics Library
	---------------------------------
	
	This is a low-level library for primitive operations on integers of
	arbitrary width.  These routines can be used to build large fixed-width
	and variable-width integer types.  Integer operands consist of arrays of
	contiguous register-sized components.  Integer extents are stored with
	native endianness at both levels -- i.e. the most significant byte of the
	most significant word is the lowest in memory on big-endian systems, and
	the highest in memory on little-endian systems.  The library provides
	separate functions for operating on big-endian and little-endian variants,
	of which the caller must select one appropriately.  Optimized assembly
	language implementations are provided for the Motorola 68K series of
	processors.
	
	Types
	  * limb_t:  A machine word of convenient size for arithmetic, generally
	             the native register width.
	  * long_t:  An unsigned integer type for a multiplication result, either
	             twice the size of limb_t, or the same size if no intrinsic
	             operation exists to multiply two limbs.
	  * twig_t:  An unsigned integer type half the size of long_t (i.e. either
	             the same size as limb_t or half the size, as above), used for
	             multiplication operands.
	  * size_t:  A limb count.  A 16-bit unsigned value allows 64K - 1 limbs,
	             which is either 256K - 4 or 512K - 8 bytes of integer data.
	  * cmp_t:   The result type of compare functions.  It's only one byte, to
	             simplify asm implementations.
	
	Constants
	  * zenith:  The maximum value representable by a limb, e.g. 0xFFFFFFFF.
	
	Function/operand naming
	  * "be", "le":     Big-endian and little-endian.
	  * "desc":         Descending, i.e. the second operand's size must not
	                    exceed the first's.
	  * "a", "b":       Commutatively interchangeable operands (except when
	                    "desc" applies and the sizes differ).
	  * "x", "y":       Destination and source operands, respectively.
	  * "low", "high":  Low and high memory.  Like begin and end, but
	                    without connotations of order.
	  * "size":         Always a count of limbs, not bytes.
	
	Functions
	  * will_carry:   Returns true if the sum of its arguments would overflow
	                  a limb, false otherwise.
	  * will_borrow:  Returns true if the difference of its arguments would
	                  underflow, false otherwise.
	  * compare:      Returns -1 for a < b, 0 for a == b, and 1 for a > b.
	  * sum_size:     Returns the size needed to store the result of adding
	                  the two operands.
	  * add:          Adds the second operand to the first one.
	  * subtract:     Subtracts the second operand from the first one.  This
	                  is strictly a cancellation function, requiring x >= y.
	  * multiply:     Multiplies the first operand by the second one.
	  * shift_right:  Shifts the operand to the right by one bit.  The most
	                  significant one bit is replaced by a zero; the least
	                  significant bit is discarded.
	
	Caveats
	  * Integer operands must contain at least one limb.
	  * It's the caller's responsibility to ensure the x operand is big enough
	    to store the result.  For add, sum_size will provide the answer.  For
	    multiply, the sum of the two operand sizes will be sufficient.  (Since
	    subtraction is strictly a diminishing operation, the result will never
	    exceed the left operand.)
*/


namespace math    {
namespace integer {
	
	typedef unsigned long long long_t;
	typedef unsigned int       twig_t;
	
	const limb_t zenith = limb_t( -1 );
	
	inline
	bool will_carry( limb_t a, limb_t b, bool carried = false )
	{
		return carried ? a >= zenith - b
		               : a >  zenith - b;
	}
	
	inline
	bool will_borrow( limb_t a, limb_t b, bool borrowed = false )
	{
		return borrowed ? a <= b
		                : a <  b;
	}
	
	/*
		Comparison
		----------
	*/
	
#ifdef __MC68K__
	
	cmp_t compare_be( limb_t const*  a_low  : __A0,
	                  size_t         a_size : __D0,
	                  limb_t const*  b_low  : __A1,
	                  size_t         b_size : __D1 );
	
#else
	
	cmp_t compare_be( limb_t const*  a_low,
	                  size_t         a_size,
	                  limb_t const*  b_low,
	                  size_t         b_size );
	
#endif
	
	cmp_t compare_le( limb_t const*  a_high,
	                  size_t         a_size,
	                  limb_t const*  b_high,
	                  size_t         b_size );
	
	inline
	cmp_t compare( bool is_little_endian,
	               limb_t const* a, size_t a_size,
	               limb_t const* b, size_t b_size )
	{
		if ( is_little_endian )
		{
			a += a_size;
			b += b_size;
			
			return compare_le( a, a_size, b, b_size );
		}
		else
		{
			return compare_be( a, a_size, b, b_size );
		}
	}
	
	/*
		Addition
		--------
	*/
	
#ifdef __MC68K__
	
	size_t sum_size_be( limb_t const* a_low : __A0, size_t a_size : __D0,
	                    limb_t const* b_low : __A1, size_t b_size : __D1 );
	
#endif
	
	size_t sum_size_desc_be( limb_t const* a_low, size_t a_size,
	                         limb_t const* b_low, size_t b_size );
	
	size_t sum_size_desc_le( limb_t const* a_high, size_t a_size,
	                         limb_t const* b_high, size_t b_size );
	
	inline
	size_t sum_size_desc( bool is_little_endian,
	                      limb_t const* a, size_t a_size,
	                      limb_t const* b, size_t b_size )
	{
		if ( is_little_endian )
		{
			a += a_size;
			b += b_size;
			
			return sum_size_desc_le( a, a_size, b, b_size );
		}
		else
		{
			return sum_size_desc_be( a, a_size, b, b_size );
		}
	}
	
	inline
	size_t sum_size( bool is_little_endian,
	                 limb_t const* a, size_t a_size,
	                 limb_t const* b, size_t b_size )
	{
	#ifdef __MC68K__
		
		return sum_size_be( a, a_size, b, b_size );
		
	#endif
		
		if ( a_size < b_size )
		{
			limb_t const* x;
			size_t n;
			
			x = a, n      = a_size;
			a = b, a_size = b_size;
			b = x, b_size = n;
		}
		
		return sum_size_desc( is_little_endian, a, a_size, b, b_size );
	}
	
#ifdef __MC68K__
	
	void add_be( limb_t*        x_high : __A0,
	             limb_t const*  y_high : __A1,
	             size_t         n      : __D1 );
	
#else
	
	void add_be( limb_t*        x_high,
	             limb_t const*  y_high,
	             size_t         n );
	
#endif
	
	void add_le( limb_t*        x_low,
	             limb_t const*  y_low,
	             size_t         n );
	
	inline
	void add( bool           is_little_endian,
	          limb_t*        x, size_t x_size,
	          limb_t const*  y, size_t y_size )
	{
		if ( is_little_endian )
		{
			add_le( x, y, y_size );
		}
		else
		{
			x += x_size;
			y += y_size;
			
			add_be( x, y, y_size );
		}
	}
	
	/*
		Subtraction
		-----------
	*/
	
#ifdef __MC68K__
	
	void subtract_be( limb_t*        x_high : __A0,
	                  limb_t const*  y_high : __A1,
	                  size_t         n      : __D1 );
	
#else
	
	void subtract_be( limb_t*        x_high,
	                  limb_t const*  y_high,
	                  size_t         n );
	
#endif
	
	void subtract_le( limb_t*        x_low,
	                  limb_t const*  y_low,
	                  size_t         n );
	
	inline
	void subtract( bool           is_little_endian,
	               limb_t*        x, size_t x_size,
	               limb_t const*  y, size_t y_size )
	{
		if ( is_little_endian )
		{
			subtract_le( x, y, y_size );
		}
		else
		{
			x += x_size;
			y += y_size;
			
			subtract_be( x, y, y_size );
		}
	}
	
	/*
		Multiplication
		--------------
	*/
	
#ifdef __MC68K__
	
	asm
	void long_multiply( long_t*  x : __A0,
	                    twig_t   a : __D0,
	                    twig_t   b : __D1 );
	
	inline
	long_t long_multiply( twig_t a, twig_t b )
	{
		long_t x;
		long_multiply( &x, a, b );
		
		return x;
	}
	
#else
	
	inline
	void long_multiply( long_t* x, twig_t a, twig_t b )
	{
		*x = (long_t) a * b;
	}
	
	inline
	long_t long_multiply( twig_t a, twig_t b )
	{
		return (long_t) a * b;
	}
	
#endif
	
	void multiply_be( limb_t*       x_low, size_t x_size,
	                  limb_t const* y_low, size_t y_size );
	
	void multiply_le( limb_t*       x_high, size_t x_size,
	                  limb_t const* y_high, size_t y_size );
	
	inline
	void multiply( bool           is_little_endian,
	               limb_t*        x, size_t x_size,
	               limb_t const*  y, size_t y_size )
	{
		if ( is_little_endian )
		{
			x += x_size;
			y += y_size;
			
			multiply_le( x, x_size, y, y_size );
		}
		else
		{
			multiply_be( x, x_size, y, y_size );
		}
	}
	
	/*
		Bit shifts
		----------
	*/
	
	void shift_right_be( limb_t* x_low, size_t n );
	void shift_right_le( limb_t* x_high, size_t n );
	
	inline
	void shift_right( bool is_little_endian, limb_t* x, size_t size )
	{
		if ( is_little_endian )
		{
			x += size;
			
			shift_right_le( x, size );
		}
		else
		{
			shift_right_be( x, size );
		}
	}
	
	/*
		Floating-point conversion
		-------------------------
	*/
	
	template < class Float >
	Float float_cast_be( limb_t const* x_low, size_t n )
	{
		Float result = 0.0;
		
		limb_t const* x = x_low;
		
		while ( n-- > 0 )
		{
			result *= ((limb_t) -1 / 2u + 1) * 2.0;
			result += *x++;
		}
		
		return result;
	}
	
	template < class Float >
	Float float_cast_le( limb_t const* x_high, size_t n )
	{
		Float result = 0.0;
		
		limb_t const* x = x_high;
		
		while ( n-- > 0 )
		{
			result *= ((limb_t) -1 / 2u + 1) * 2.0;
			result += *--x;
		}
		
		return result;
	}
	
	template < class Float >
	inline
	Float float_cast( bool is_little_endian, limb_t const* x, size_t size )
	{
		if ( is_little_endian )
		{
			x += size;
			
			return float_cast_le< Float >( x, size );
		}
		else
		{
			return float_cast_be< Float >( x, size );
		}
	}
	
}
}

#endif
