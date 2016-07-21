/*
	integer.cc
	----------
*/

#include "plus/integer.hh"

// relix-compat
#include "relix/recurse.hh"


namespace plus
{
	
	integer& integer::operator+=( const integer& y )
	{
		integer& x = *this;
		
		if ( y.is_zero() )  return x;
		if ( x.is_zero() )  return x = y;
		
		const bool x_neg = x.is_negative();
		const bool y_neg = y.is_negative();
		
		if ( x_neg == y_neg )
		{
			box.add( y.box );
		}
		else
		{
			box.subtract( y.box );
		}
		
		return x;
	}
	
	integer& integer::operator-=( const integer& y )
	{
		return *this += -y;
	}
	
	integer& integer::operator*=( const integer& y )
	{
		integer& x = *this;
		
		if ( x.is_zero() )  return x;
		if ( y.is_zero() )  return x = y;
		
		box.multiply_by( y.box );
		
		return x;
	}
	
/*	
	It's very unfortunate that we have to specify the exact instantiation of
	relix::recurse<>(), but if we don't, then Metrowerks C++ will infer type
	`const integer` instead of `const integer&` and generate useless copies.
*/
	
	template < class F >
	static inline void recurse( F f, const integer& a, integer& b, integer& c )
	{
		typedef const integer&  A;
		typedef       integer&  B;
		typedef       integer&  C;
		
		relix::recurse< void, A, A, B, B, C, C >( f, a, b, c );
	}
	
/*
	The internal division routine leaves the remainder in the `dividend`
	parameter.  If the divisor exceeds the dividend, it does nothing.
	Otherwise, it recursively calls itself with a doubled divisor and then
	doubles the resulting quotient.  If the remainder exceeds or equals the
	divisor, it subtracts the divisor from the remainder and increments the
	quotient.
	
	Since the difference in magnitude between the dividend and the divisor
	(which is the only limit on the recursion depth) can be especially large
	(over 260,000 for a 32-bit limb and 16-bit count (if division by 2 isn't
	optimized)), we need to make sure that we don't destroy the system by
	overrunning the stack.  Assuming a 64-byte stack frame, the worst case
	for division will require sixteen megabytes.  Modern operating systems
	will either page memory onto the end of the stack on demand, or kill the
	process (either of which is an acceptable outcome from this library's
	perspective -- if you want to perform division on numbers that only fit
	in hundreds of kilobytes, it's on you to make sure your OS is up to it,
	e.g. via ulimit).
	
	Less capable operating systems are at high risk, however.  If a program
	running in MacRelix in classic Mac OS runs off the end of the stack, it
	could not only corrupt or crash the entire MacRelix process, but even
	the whole underlying Mac OS system, possibly requiring a hard reset.
	
	To mitigate this, we recurse indirectly through relix::recurse(), which
	checks the available stack space, and if it's running low, allocates a
	new stack and switches to it before calling the recursive function.
	(The memory allocation can fail, of course, in which case SIGSTKFLT is
	generated, and the process is killed unless it catches the signal.)
*/
	
	static
	void divide( const integer&  divisor,
	             integer&        dividend,
	             integer&        quotient )
	{
		if ( abs_compare( dividend, divisor ) >= 0 )
		{
			recurse( &divide, divisor + divisor, dividend, quotient );
			
			quotient += quotient;
			
			if ( abs_compare( dividend, divisor ) >= 0 )
			{
				++quotient;
				
				dividend -= divisor;
			}
		}
		
		// remainder is in `dividend`
	}
	
	static inline
	bool builtin_division_usable( const integer& divisor,
	                              const integer& dividend )
	{
		const unsigned long max = (unsigned long) -1;
		
		return divisor.is_positive()  &&  divisor <= max  &&  dividend <= max;
	}
	
	static
	integer divide( const integer&  divisor,
	                integer&        dividend )
	{
		if ( divisor .is_zero() )  throw division_by_zero();
		if ( dividend.is_zero() )  return dividend;
		
		integer quotient;
		
		if ( divisor == 1 )
		{
			// For x / 1, the quotient is x and the remainder is zero.
			swap( dividend, quotient );
		}
		else
		{
			// 3-arg divide() requires its inputs to have the same sign.
			
			const bool was_negative = dividend.is_negative();
			
			if ( was_negative != divisor.is_negative() )
			{
				dividend.invert();
			}
			
			if ( builtin_division_usable( divisor, dividend ) )
			{
				const unsigned long divid = dividend.clipped();
				const unsigned long divis = divisor .clipped();
				
				dividend = divid % divis;  // remainder
				quotient = divid / divis;
			}
			else
			{
				divide( divisor, dividend, quotient );
			}
			
			/*
				 7 /  3 ->  2r1
				 7 / -3 -> -2r1
				-7 /  3 -> -2r-1
				-7 / -3 ->  2r-1
				
				The quotient always comes out nonnegative, so if the signs
				of the dividend and divisor differ, then we have to invert it.
				The remainder (if not zero) will have the same sign as the
				inverted dividend (which is to say, the same as the divisor),
				so we have to invert it back.
			*/
			
			if ( was_negative != divisor.is_negative() )
			{
				dividend.invert();
				quotient.invert();
			}
		}
		
		// remainder is in `dividend`
		
		return quotient;
	}
	
	integer& integer::operator/=( const integer& y )
	{
		return *this = divide( y, *this );
	}
	
	integer& integer::operator%=( const integer& y )
	{
		divide( y, *this );
		
		return *this;
	}
	
	integer& integer::modulo_by( const integer& modulus )
	{
		divide( modulus, *this );
		
		if ( -sign() == modulus.sign() )
		{
			*this += modulus;
		}
		
		return *this;
	}
	
	integer raise_to_power( integer base, integer exponent )
	{
		if ( exponent.is_negative() )
		{
			throw negative_exponent();
		}
		
		integer result = 1;
		
		while ( ! exponent.is_zero() )
		{
			if ( exponent.is_odd() )
			{
				result *= base;
			}
			
			base *= base;
			
			exponent.halve();
		}
		
		return result;
	}
	
}
