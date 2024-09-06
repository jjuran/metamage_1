/*
	pjw-hash.cc
	-----------
	
	This is an improved version of the PJW hash function.
	
	https://en.wikipedia.org/wiki/PJW_hash_function
	
	The original has a conditional block that only executes
	when the top eighth of the bits are collectively non-zero;
	this version recognizes that the two operations which are
	conditionalized are no-ops when the condition is false --
	so there's no need to actually test the condition.
	
	Furthermore, the second of those two operations clears
	the very same high bits that are shifted out in the next
	iteration of the loop, so that operation can be hoisted
	out of the loop and performed only once, at the end --
	except that discards bits from the hash for no good reason.
	This version doesn't do that; if the caller prefers a hash
	that's merely 28 bits, it can arrange that on its own.
	
*/

#include "pjw-hash/pjw-hash.hh"


namespace pjw_hash
{

enum
{
	n_top_bits = sizeof (hash_t) * 8     / 8,  // for 32-bit,  4 bits
	n_shifting = sizeof (hash_t) * 8 * 3 / 4,  // for 32-bit, 24 bits
	
	top_mask = ~((hash_t) -1 >> n_top_bits),
};

#ifdef __MC68K__

asm
hash_t function( const char* s : __A0 )
{
	MOVEQ    #0,D0
	MOVEQ    #0,D1
	
loop:
	MOVE.B   (A0)+,D1
	BEQ.S    done
	
	LSL.L    #4,D0
	ADD.L    D1,D0
	
	MOVE.L   D0,D2
	ROL.L    #8,D2
	ANDI.W   #0x00F0,D2
	EOR.W    D2,D0
	
	BRA.S    loop
	
done:
	
	RTS
}

asm
hash_t function( const byte_t* p : __A0, size_t n_bytes : __D0 )
{
	MOVEA.L  A0,A1
	ADDA.L   D0,A1
	
	MOVEQ    #0,D0
	MOVEQ    #0,D1
	
loop:
	MOVE.B   (A0)+,D1
	LSL.L    #4,D0
	ADD.L    D1,D0
	
	MOVE.L   D0,D2
	ROL.L    #8,D2
	ANDI.W   #0x00F0,D2
	EOR.W    D2,D0
	
	CMPA.L   A0,A1
	BNE.S    loop
	
	RTS
}

#else

hash_t function( const char* s )
{
	hash_t hash = 0;
	
	while ( byte_t c = *s++ )
	{
		hash <<= n_top_bits;
		hash += c;
		
		hash ^= (hash & top_mask) >> n_shifting;
	}
	
	return hash;
}

hash_t function( const byte_t* p, size_t n_bytes )
{
	hash_t hash = 0;
	
	const byte_t* end = p + n_bytes;
	
	while ( p < end )
	{
		hash <<= n_top_bits;
		hash += *p++;
		
		hash ^= (hash & top_mask) >> n_shifting;
	}
	
	return hash;
}

#endif

}
