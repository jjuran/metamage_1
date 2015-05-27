/*
	math/integer.hh
	---------------
*/

#ifndef MATH_INTEGER_HH
#define MATH_INTEGER_HH

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
	separate functions for operating on big-endian and little-endian varants,
	of which the caller must select one appropriately.  Optimized assembly
	language implementations are provided for the Motorola 68K series of
	processors.
	
	Types
	  * limb_t:  A machine word of convenient size for arithmetic, generally
	             the native register width.
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
	  * carries:   Returns true if the sum of its arguments would overflow a
	               limb, false otherwise.
	  * compare:   Returns -1 for a < b, 0 for a == b, and 1 for a > b.
	  * sum_size:  Returns the size needed to store the result of adding the
	               two operands.
	  * add:       Adds the second operand to the first one.
	
	Caveats
	  * Integer operands must contain at least one limb.
*/


namespace math    {
namespace integer {
	
	typedef unsigned long   limb_t;
	typedef unsigned short  size_t;
	typedef signed char     cmp_t;
	
	const limb_t zenith = limb_t( -1 );
	
	static inline bool carries( limb_t a, limb_t b, bool carried = false )
	{
		return carried ? a >= zenith - b
		               : a >  zenith - b;
	}
	
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
	
#ifdef __MC68K__
	
	size_t sum_size_desc_be( limb_t const* a_low : __A0, size_t a_size : __D0,
	                         limb_t const* b_low : __A1, size_t b_size : __D1 );
	
#else
	
	size_t sum_size_desc_be( limb_t const* a_low, size_t a_size,
	                         limb_t const* b_low, size_t b_size );
	
#endif
	
	size_t sum_size_desc_le( limb_t const* a_high, size_t a_size,
	                         limb_t const* b_high, size_t b_size );
	
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
	
}
}

#endif
