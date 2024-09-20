/*
	fractions.hh
	------------
*/

#ifndef MATH_FRACTIONS_HH
#define MATH_FRACTIONS_HH


namespace math      {
namespace fractions {

#ifdef __MC68K__

/*
	For unsigned division by a power of two, MWC68K generates
	a single LSR instruction, which can't be improved upon.
	
	For signed division by powers of two, MWC68K generates
	code like the following:
	
	For division by 2:
	
		MOVE.L   D6,D1
		LSR.L    #8,D1
		LSR.L    #8,D1
		LSR.L    #8,D1
		LSR.L    #7,D1
		ADD.L    D6,D1
		ASR.L    #1,D1
	
	For division by 4:
	
		MOVE.L   A0,D0
		ASR.L    #1,D0
		LSR.L    #8,D0
		LSR.L    #8,D0
		LSR.L    #8,D0
		LSR.L    #6,D0
		ADD.L    A0,D0
		ASR.L    #2,D0
	
	Consider the case of division by two.  The LSR instructions shift by
	a total of 31 bits, preserving only the sign bit, now in the least
	significant position.  Note that for non-negative values, this bit
	is zero, and the sequence of four LSRs and an ADD amounts to an
	expensive no-op on the way to the actually meaningful bit shift.
	
	For negative values, however, adding the sign bit is significant.
	While it makes no difference for even values, for odd values it
	matters:  For example, (-3 / 2) == -1, but (-3 >> 1) == -2.  To
	use ASR in place of DIVS, we need to add one to odd negative values,
	either before or after the bit shift.  (We can also add one to even
	values before the shift without affecting the result, but not after.)
	
	One positive feature of this approach is that it's branchless; on the
	other hand, the four LSR instructions could have been replaced with
	one ROL and an AND, shifting bits only one position instead of 31.
	(Note that on a 68000, each bit position shifted has a cost.)
	
	Alternative division by 2:
	
		MOVE.L   D6,D1
		ROL.L    #1,D1
		AND.L    #0x00000001,D1
		ADD.L    D6,D1
		ASR.L    #1,D1
	
	While it's fewer instructions (and faster), due to the longword-sized
	immediate operand, the code is the same number of words as before.
	
	Like the Metrowerks-generated code and the alternate version shown
	above, we'll add one to negative values prior to the bit shift.
	However, instead of shifting (or rotating) the sign bit into place,
	we'll use a branch:
	
		MOVE.L   D6,D0
		BPL.S    no_add
		ADDQ.L   #1,D0
	no_add:
		ASR.L    #1,D0
	
	Ignoring the initial move to the target data register, this code is
	three words vs. Metrowerks' six.  Instead of rotate-AND-add-shift,
	(or even shift-shift-shift-shift-add-shift, which is plain silly),
	we have either branch-shift or nonbranch-add-shift.
	
	We assume that each invocation is immediately preceded by a move of
	the value to the target register, which populates the CCR.
	
	Possible future directions include implementation for divisors other
	than two, and for 64-bit operands.
*/

#pragma parameter half( __D0 )

inline
asm
signed char half( signed char x )
{
	BPL.S    no_add
	
	ADDQ.B   #1,D0
	
no_add:
	ASR.B    #1,D0
}

#pragma parameter half( __D0 )

inline
asm
signed short half( signed short x )
{
	BPL.S    no_add
	
	ADDQ.W   #1,D0
	
no_add:
	ASR.W    #1,D0
}

#pragma parameter half( __D0 )

inline
asm
signed long half( signed long x )
{
	BPL.S    no_add
	
	ADDQ.L   #1,D0
	
no_add:
	ASR.L    #1,D0
}

#pragma parameter half( __D0 )

inline
signed int half( signed int x )
{
	return __option( fourbyteints ) ? half( (long)  x )
	                                : half( (short) x );
}

#pragma parameter div_65536_ULL( __A0 )

inline
asm
unsigned long long* div_65536_ULL( unsigned long long* x )
{
	MOVE.L   (A0)+,D1
	MOVE.L   (A0),D0
	
	MOVE.W   D1,D0
	SWAP     D0
	
	CLR.W    D1
	SWAP     D1
	
	MOVE.L   D0,(A0)
	MOVE.L   D1,-(A0)
}

#else

inline
signed char half( signed char x )
{
	return x / 2;
}

inline
signed short half( signed short x )
{
	return x / 2;
}

inline
signed int half( signed int x )
{
	return x / 2;
}

inline
signed long half( signed long x )
{
	return x / 2;
}

inline
unsigned long long* div_65536_ULL( unsigned long long* x )
{
	*x /= 65536;
	
	return x;
}

#endif

inline
signed long long half( signed long long x )
{
	return x / 2;
}


inline
unsigned char half( unsigned char x )
{
	return x / 2;
}

inline
unsigned short half( unsigned short x )
{
	return x / 2;
}

inline
unsigned int half( unsigned int x )
{
	return x / 2;
}

inline
unsigned long half( unsigned long x )
{
	return x / 2;
}

inline
unsigned long long half( unsigned long long x )
{
	return x / 2;
}

}
}

#endif
