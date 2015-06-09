/*
	math/integer.cc
	---------------
*/

#include "math/integer.hh"


namespace math    {
namespace integer {
	
	/*
		Comparison
		----------
	*/
	
#ifdef __MC68K__
	
	asm
	cmp_t compare_be( limb_t const*  a_low  : __A0,
	                  size_t         a_size : __D0,
	                  limb_t const*  b_low  : __A1,
	                  size_t         b_size : __D1 )
	{
		LINK     A6,#0
		
		// Compare the sizes.
		
		CMP.W    D1,D0
		BNE.S    compared
		
		// Compare contents, from most to least significant.
		
		SUBQ.W   #1,D1
	loop:
		CMPM.L   (A1)+,(A0)+
		DBNE.S   D1,loop
		
		// Set the result to -1, 0, or 1.
		
	compared:
		SCS      D0
		BCS.S    end
		
		SNE      D0
		NEG.B    D0
		
	end:
		UNLK     A6
		RTS
	}
	
#else
	
	cmp_t compare_be( limb_t const*  a_low,
	                  size_t         a_size,
	                  limb_t const*  b_low,
	                  size_t         b_size )
	{
		if ( const cmp_t cmp = (a_size > b_size) - (a_size < b_size) )
		{
			return cmp;
		}
		
		limb_t const* p = a_low;
		limb_t const* q = b_low;
		
		size_t n = b_size;
		
		do
		{
			const limb_t a = *p++;
			const limb_t b = *q++;
			
			if ( a < b )  return -1;
			if ( a > b )  return  1;
		}
		while ( --n > 0 );
		
		return 0;
	}
	
#endif
	
	cmp_t compare_le( limb_t const*  a_high,
	                  size_t         a_size,
	                  limb_t const*  b_high,
	                  size_t         b_size )
	{
		if ( const cmp_t cmp = (a_size > b_size) - (a_size < b_size) )
		{
			return cmp;
		}
		
		limb_t const* p = a_high;
		limb_t const* q = b_high;
		
		size_t n = b_size;
		
		do
		{
			const limb_t a = *--p;
			const limb_t b = *--q;
			
			if ( a < b )  return -1;
			if ( a > b )  return  1;
		}
		while ( --n > 0 );
		
		return 0;
	}
	
	/*
		Addition
		--------
	*/
	
#ifdef __MC68K__

	asm
	size_t sum_size_be( limb_t const* a_low : __A0, size_t a_size : __D0,
	                    limb_t const* b_low : __A1, size_t b_size : __D1 )
	{
		LINK     A6,#0
		
		// Swap a and b if in ascending order.
		
		CMP.W    D1,D0
		BCC.S    descending
		EXG      A0,A1
		EXG      D0,D1
	descending:
		
		// While b is zero, compare a against 0xFFFFFFFF.
		
		MOVE.W   D0,D2
		SUB.W    D1,D2
		BRA.S    a_only_loop_start
	a_only_loop:
		CMPI.L   #-1,(A0)+
		BNE.S    end
	a_only_loop_start:
		DBRA.S   D2,a_only_loop
		
		// Compare contents until we know adding will or won't carry.
		
		SUBQ.W   #1,D1
	loop:
		MOVE.L   (A0)+,D2
		ADD.L    (A1)+,D2
		BCS.S    carry
		ADDQ.L   #1,D2
		DBNE.S   D1,loop
		
		// Return the size.
		
		BRA.S    end
	carry:
		ADDQ.W   #1,D0
	end:
		UNLK     A6
		RTS
	}
	
#else
	
	size_t sum_size_desc_be( limb_t const* a_low, size_t a_size,
	                         limb_t const* b_low, size_t b_size )
	{
		limb_t const* p = a_low;
		limb_t const* q = b_low;
		
		size_t n = a_size;
		
		while ( n > b_size )
		{
			if ( *p++ != zenith )
			{
				return a_size;
			}
			
			--n;
		}
		
		do
		{
			const limb_t a = *p++;
			const limb_t b = *q++;
			
			if ( a + b != zenith )
			{
				return a_size + will_carry( a, b );
			}
		}
		while ( --n > 0 );
		
		return a_size;
	}
	
#endif
	
	size_t sum_size_desc_le( limb_t const* a_high, size_t a_size,
	                         limb_t const* b_high, size_t b_size )
	{
		limb_t const* p = a_high;
		limb_t const* q = b_high;
		
		size_t n = a_size;
		
		while ( n > b_size )
		{
			if ( *--p != zenith )
			{
				return a_size;
			}
			
			--n;
		}
		
		do
		{
			const limb_t a = *--p;
			const limb_t b = *--q;
			
			if ( a + b != zenith )
			{
				return a_size + will_carry( a, b );
			}
		}
		while ( --n > 0 );
		
		return a_size;
	}
	
#ifdef __MC68K__
	
	asm
	void add_be( limb_t*        x_high : __A0,
	             limb_t const*  y_high : __A1,
	             size_t         n      : __D1 )
	{
		LINK     A6,#0
		
		// Add the contents, from least to most significant.
		
		SUBQ.W   #1,D1
	add_loop:
		ADDX.L   -(A1),-(A0)
		DBRA.S   D1,add_loop
		
		// The second operand is exhausted.
		
		BCC.S    end
		
		// Add the carry bit.
		
	carry_loop:
		ADDQ.L   #1,-(A0)
		BCS.S    carry_loop
		
	end:
		UNLK     A6
		RTS
	}
	
#else
	
	void add_be( limb_t*        x_high,
	             limb_t const*  y_high,
	             size_t         n )
	{
		limb_t* r = x_high;
		
		limb_t const* p = x_high;
		limb_t const* q = y_high;
		
		bool carrying = false;
		
		do
		{
			const limb_t a = *--p;
			const limb_t b = *--q;
			
			const limb_t sum = a + b + carrying;
			
			*--r = sum;
			
			carrying = will_carry( a, b, carrying );
		}
		while ( --n > 0 );
		
		while ( carrying )
		{
			const limb_t a = *--p + 1;
			
			*--r = a;
			
			carrying = a == 0;
		}
	}
	
#endif
	
	void add_le( limb_t*        x_low,
	             limb_t const*  y_low,
	             size_t         n )
	{
		limb_t* r = x_low;
		
		limb_t const* p = x_low;
		limb_t const* q = y_low;
		
		bool carrying = false;
		
		do
		{
			const limb_t a = *p++;
			const limb_t b = *q++;
			
			const limb_t sum = a + b + carrying;
			
			*r++ = sum;
			
			carrying = will_carry( a, b, carrying );
		}
		while ( --n > 0 );
		
		while ( carrying )
		{
			const limb_t a = *p++ + 1;
			
			*r++ = a;
			
			carrying = a == 0;
		}
	}
	
	/*
		Subtraction
		-----------
	*/
	
#ifdef __MC68K__
	
	asm
	void subtract_be( limb_t*        x_high : __A0,
	                  limb_t const*  y_high : __A1,
	                  size_t         n      : __D1 )
	{
		LINK     A6,#0
		
		// Subtract the contents, from least to most significant.
		
		SUBQ.W   #1,D1
	subtract_loop:
		SUBX.L   -(A1),-(A0)
		DBRA.S   D1,subtract_loop
		
		// The second operand is exhausted.
		
		BCC.S    end
		
		// Subtract the carry bit.
		
	borrow_loop:
		SUBQ.L   #1,-(A0)
		BCS.S    borrow_loop
		
	end:
		UNLK     A6
		RTS
	}
	
#else
	
	void subtract_be( limb_t*        x_high,
	                  limb_t const*  y_high,
	                  size_t         n )
	{
		limb_t* r = x_high;
		
		limb_t const* p = x_high;
		limb_t const* q = y_high;
		
		bool borrowing = false;
		
		do
		{
			const limb_t a = *--p;
			const limb_t b = *--q;
			
			const limb_t diff = a - b - borrowing;
			
			*--r = diff;
			
			borrowing = will_borrow( a, b, borrowing );
		}
		while ( --n > 0 );
		
		while ( borrowing )
		{
			const limb_t a = *--p;
			
			borrowing = a == 0;
			
			*--r = a - 1;
		}
	}
	
#endif
	
	void subtract_le( limb_t*        x_low,
	                  limb_t const*  y_low,
	                  size_t         n )
	{
		limb_t* r = x_low;
		
		limb_t const* p = x_low;
		limb_t const* q = y_low;
		
		bool borrowing = false;
		
		do
		{
			const limb_t a = *p++;
			const limb_t b = *q++;
			
			const limb_t diff = a - b - borrowing;
			
			*r++ = diff;
			
			borrowing = will_borrow( a, b, borrowing );
		}
		while ( --n > 0 );
		
		while ( borrowing )
		{
			const limb_t a = *p++;
			
			borrowing = a == 0;
			
			*r++ = a - 1;
		}
	}
	
	/*
		Multiplication
		--------------
	*/
	
	typedef unsigned long long long_t;
	typedef unsigned int       twig_t;
	
	const unsigned twigs_per_limb = sizeof (limb_t) / sizeof (twig_t);
	const unsigned limbs_per_long = sizeof (long_t) / sizeof (limb_t);
	
	static inline
	long_t long_multiply( twig_t a, twig_t b )
	{
		return (long_t) a * b;
	}
	
	void multiply_be( limb_t*       x_low, size_t x_size,
	                  limb_t const* y_low, size_t y_size )
	{
		twig_t* p = (twig_t*) x_low;
		
		for ( int i = x_size * twigs_per_limb;  i > 0;  --i )
		{
			if ( const twig_t a = *p++ )
			{
				p[ -1 ] = 0;
				
				twig_t const* q = (twig_t const*) y_low;
				
				for ( int j = y_size * twigs_per_limb - 1;  j >= 0;  --j )
				{
					if ( const twig_t b = *q++ )
					{
						const long_t product = long_multiply( a, b );
						
						add_be( (limb_t*) (p - j),
						        (limb_t const*) (&product + 1),
						        limbs_per_long );
					}
				}
			}
		}
	}
	
	void multiply_le( limb_t*       x_high, size_t x_size,
	                  limb_t const* y_high, size_t y_size )
	{
		const bool needs_alignment = sizeof (twig_t) < sizeof (limb_t);
		
		twig_t* p = (twig_t*) x_high;
		
		bool p_is_aligned = true;
		
		for ( int i = x_size * twigs_per_limb;  i > 0;  --i )
		{
			if ( needs_alignment )
			{
				p_is_aligned = ! p_is_aligned;
			}
			
			if ( const long_t a = *--p )
			{
				*p = 0;
				
				twig_t const* q = (twig_t const*) y_high;
				
				for ( int j = y_size * twigs_per_limb - 1;  j >= 0;  --j )
				{
					if ( const twig_t b = *--q )
					{
						const long_t product = long_multiply( a, b );
						
						if ( needs_alignment  &&  p_is_aligned == (j & 1) )
						{
							const int n_bits = sizeof (twig_t) * 8;
							
							if ( const limb_t upper = product >> n_bits )
							{
								add_le( (limb_t*) (p + j + 1), &upper, 1 );
							}
							
							if ( const limb_t lower = product << n_bits )
							{
								add_le( (limb_t*) (p + j - 1), &lower, 1 );
							}
							
							continue;
						}
						
						add_le( (limb_t*) (p + j),
						        (limb_t const*) &product,
						        limbs_per_long );
					}
				}
			}
		}
	}
	
}
}
