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
		DBNE     D1,loop
		
		// Set the result to -1, 0, or 1.
		
	compared:
		SCS      D0
		BLS.S    end
		
		MOVEQ.L  #1,D0
		
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
		DBRA     D2,a_only_loop
		
		// Compare contents until we know adding will or won't carry.
		
		SUBQ.W   #1,D1
	loop:
		MOVE.L   (A0)+,D2
		ADD.L    (A1)+,D2
		BCS.S    carry
		ADDQ.L   #1,D2
		DBNE     D1,loop
		
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
		DBRA     D1,add_loop
		
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
		DBRA     D1,subtract_loop
		
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
	
	const unsigned twigs_per_limb = sizeof (limb_t) / sizeof (twig_t);
	const unsigned limbs_per_long = sizeof (long_t) / sizeof (limb_t);
	
#ifdef __MC68K__
	
	asm
	void long_multiply( long_t*  x : __A0,
	                    twig_t   a : __D0,
	                    twig_t   b : __D1 )
	{
		/*
			A 68020 or later implements longword multiplication in a single
			instruction, which we'd like to use if available.  If codegen is
			specifically targeting 68020 (or later), then we can just use it.
			Otherwise, we'll test for it (though obliquely) -- just because
			our build is inclusive of the 68000 doesn't mean we don't want to
			have the advantage on a 68020+.
		*/
		
	#if ! __MC68020__
		
		/*
			This is an inclusive build, so we check.  A PC-relative jump has
			an offset relative to the word following the beginning of the JMP
			instruction (which is the /middle/ of the instruction, not the end
			of it).  So an offset of 2 jumps to the end of the JMP instruction
			(to the subsequent BRA) and an offset of 4 jumps over the BRA and
			directly to the 68020-only MULU.L.  The jump's effective address
			contains a scaling factor which is only recognized by a 68020+, so
			the scaled offset is 4 in that case but remains 2 on a 68000.  In
			the latter event, we branch over the 68020+ block and calculate
			via FOIL as we did previously.
		*/
		
		MOVEQ.L  #2,D2
		
	#if __MWERKS__ > 0x2200
		JMP      (PC, D2.W*2)
	#else
		DC.L     0x4efb2200  // JMP (PC, D2.W*2)
	#endif
		
		BRA.S    foil
		
	#endif
		
		machine  68020
		
		MULU.L   D0,D2:D1  // a * b -> D2 (high) and D1 (low)
		
		MOVE.L   D2,(A0)+
		MOVE.L   D1,(A0)
		
		RTS
	
	#if ! __MC68020__
		
	foil:
		
		/*
			Multiply 32-bit operands using a FOIL (first/outer/inner/last)
			strategy.  Instead of first and last, we have upper and lower
			words, which we'll distinguish in the comments by letter case:
			A, a, B, and b.  The result is just X, with the portion in use
			at any given step commented explicitly.
		*/
		
		MOVE.L   D0,D2
		MULU.W   D1,D2
		MOVE.L   D2,4(A0)  // a * b -> X (low longword)
		
		MOVEA.L  D0,A1     // save a for later
		SWAP     D0        // load A
		MOVE.L   D0,D2
		MULU.W   D1,D2     // A * b -> D2
		SWAP     D1        // load B
		MULU.W   D1,D0
		MOVE.L   D0,(A0)   // A * B -> X (high longword)
		
		MOVE.L   A1,D0     // load a
		MULU.W   D1,D0     // a * B -> D0
		
		MOVEQ.L  #0,D1
		ADD.L    D2,D0     // add (A * b) to (a * B)
		ADDX.W   D1,D1     // save carry bit in D1
		
		MOVEQ.L  #0,D2
		ADD.L    D0,2(A0)  // add (A * b) + (a * B) to X (middle)
		ADDX.W   D2,D1     // add carry bit to D1
		
		ADD.W    D1,(A0)   // add the carries to X (high longword)
		
		RTS
		
	#endif  // #if ! __MC68020__
	}
	
#endif
	
	void multiply_be( limb_t*       x_low, size_t x_size,
	                  limb_t const* y_low, size_t y_size )
	{
		const bool needs_alignment = sizeof (twig_t) < sizeof (limb_t);
		
		twig_t* p = (twig_t*) x_low;
		
		bool p_is_aligned = true;
		
		for ( int i = x_size * twigs_per_limb;  i > 0;  --i )
		{
			if ( needs_alignment )
			{
				p_is_aligned = ! p_is_aligned;
			}
			
			if ( const twig_t a = *p++ )
			{
				p[ -1 ] = 0;
				
				twig_t const* q = (twig_t const*) y_low;
				
				for ( int j = y_size * twigs_per_limb - 1;  j >= 0;  --j )
				{
					if ( const twig_t b = *q++ )
					{
						long_t product;
						
						long_multiply( &product, a, b );
						
						if ( needs_alignment  &&  p_is_aligned == (j & 1) )
						{
							const int n_bits = sizeof (twig_t) * 8;
							
							if ( const limb_t upper = product >> n_bits )
							{
								add_be( (limb_t*) (p - j - 1), &upper + 1, 1 );
							}
							
							if ( const limb_t lower = product << n_bits )
							{
								add_be( (limb_t*) (p - j + 1), &lower + 1, 1 );
							}
							
							continue;
						}
						
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
						long_t product;
						
						long_multiply( &product, a, b );
						
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
	
	/*
		Bit shifts
		----------
	*/
	
	void shift_right_be( limb_t* x_low, size_t n )
	{
		const int n_bits = sizeof (limb_t) * 8;
		
		limb_t* x = x_low;
		
		limb_t carried_bit = 0;
		
		while ( n-- > 0 )
		{
			limb_t lsb = *x & 1;
			
			*x >>= 1;
			
			*x++ |= carried_bit << (n_bits - 1);
			
			carried_bit = lsb;
		}
	}
	
	void shift_right_le( limb_t* x_high, size_t n )
	{
		const int n_bits = sizeof (limb_t) * 8;
		
		limb_t* x = x_high;
		
		limb_t carried_bit = 0;
		
		while ( n-- > 0 )
		{
			limb_t lsb = *--x & 1;
			
			*x >>= 1;
			
			*x |= carried_bit << (n_bits - 1);
			
			carried_bit = lsb;
		}
	}
	
}
}
