/*
	math/bitwise.hh
	---------------
*/

#ifndef MATH_BITWISE_HH
#define MATH_BITWISE_HH

/*
	Types
	  * size_t:  A limb count.  A 32-bit unsigned value allows 2^32 - 1 bytes.
	
	Function/operand naming
	  * "x", "y":  Destination and source operands, respectively.
	  * "n":       Always a count of limbs, not bytes.
	
	Functions
	  * bitwise_and:  Clears bits in x that are clear in y.
	  * bitwise_or:   Sets bits in x that are set in y.
	  * bitwise_xor:  Toggles bits in x that are set in y.
	  * bitwise_not:  Toggles all bits in x.
*/


namespace math    {
namespace bitwise {
	
	typedef unsigned  size_t;
	
	/*
		Bit logic
		---------
	*/
	
	template < class Limb >
	void bitwise_and( Limb* x, Limb const* y, size_t n )
	{
		while ( n-- > 0 )
		{
			*x++ &= *y++;
		}
	}
	
	template < class Limb >
	void bitwise_or( Limb* x, Limb const* y, size_t n )
	{
		while ( n-- > 0 )
		{
			*x++ |= *y++;
		}
	}
	
	template < class Limb >
	void bitwise_xor( Limb* x, Limb const* y, size_t n )
	{
		while ( n-- > 0 )
		{
			*x++ ^= *y++;
		}
	}
	
	template < class Limb >
	void bitwise_not( Limb* x, size_t n )
	{
		while ( n-- > 0 )
		{
			*x = ~*x;
			++x;
		}
	}
	
}
}

#endif
