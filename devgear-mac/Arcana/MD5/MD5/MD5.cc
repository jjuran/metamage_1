/*	======
 *	MD5.cc
 *	======
 */

// See RFC 1321, "The MD5 Message-Digest Algorithm".
// <http://www.faqs.org/rfcs/rfc1321.html>

#include <algorithm>
#include <cmath>
#include <vector>

// iota
#include "iota/endian.hh"

#include "MD5/MD5.hh"


namespace MD5
{
	
	using iota::swap_4_bytes;
	using iota::little_u32;
	using iota::u32_from_little;
	
	
	class Table
	{
		private:
			u32 data[ 64 ];
		
		public:
			Table();
			
			u32 operator[]( int i ) const  { return data[ i ]; }
	};
	
	static Table gTable;
	
	Table::Table()
	{
		using std::abs;
		using std::sin;
		
		for ( int i = 1;  i <= 64;  ++i )
		{
			data[ i - 1 ] = u32( 4294967296.0 * abs( sin( double( i ) ) ) );
		}
	}
	
	Buffer::Buffer() : a( swap_4_bytes( 0x01234567 ) ),
	                   b( swap_4_bytes( 0x89abcdef ) ),
	                   c( swap_4_bytes( 0xfedcba98 ) ),
	                   d( swap_4_bytes( 0x76543210 ) )
	{
	}
	
#ifdef __MC68K__
	
	static
	inline asm u32 rotate_left( u32 x : __D0, unsigned char n : __D1 ) : __D0
	{
		ROL.L  D1,D0
	}
	
#else
	
	static inline u32 rotate_left( u32 x, unsigned char bits )
	{
		return (x << bits) | (x >> (32 - bits));
	}
	
#endif
	
	static inline u32 F( u32 x, u32 y, u32 z )  { return (x & y)  |  (~x & z); }
	static inline u32 G( u32 x, u32 y, u32 z )  { return (x & z)  |  (y & ~z); }
	static inline u32 H( u32 x, u32 y, u32 z )  { return (x ^ y ^ z); }
	static inline u32 I( u32 x, u32 y, u32 z )  { return (y ^ (x | ~z)); }
	
	
	template < int round >
	struct munger;
	
	template <>
	struct munger< 1 >
	{
		static u32 f( u32 x, u32 y, u32 z )  { return F( x, y, z ); }
	};
	
	template <>
	struct munger< 2 >
	{
		static u32 f( u32 x, u32 y, u32 z )  { return G( x, y, z ); }
	};
	
	template <>
	struct munger< 3 >
	{
		static u32 f( u32 x, u32 y, u32 z )  { return H( x, y, z ); }
	};
	
	template <>
	struct munger< 4 >
	{
		static u32 f( u32 x, u32 y, u32 z )  { return I( x, y, z ); }
	};
	
	
	template < int round >
	static u32 mix( u32 const*  block,
	                u32         w,
	                u32         x,
	                u32         y,
	                u32         z,
	                int         k,
	                int         s,
	                int         i )
	{
		w += munger< round >::f( x, y, z ) + block[ k ] + gTable[ i ];
		
		return x + rotate_left( w, s );
	}
	
#ifdef __MC68K__
	
	static
	asm void do_rounds( u32* const block : __A0, Buffer& buffer : __A1 )
	{
		LINK     A6,#0
		MOVEM.L  D3-D7/A2-A4,-(SP)
		
		MOVE.L   (A1)+,D0
		MOVE.L   (A1)+,D1
		MOVE.L   (A1)+,D2
		MOVE.L   (A1)+,D3
		
		LEA      gTable,A2
		
		MOVEQ.L  #0,D7
		
	round1:
		MOVEQ.L  #7,D6
		
	round1_inner:
		// F
		MOVE.L   D1,D4
		AND.L    D2,D4
		MOVE.L   D1,D5
		NOT.L    D5
		AND.L    D3,D5
		OR.L     D5,D4
		
		ADD.L    D4,D0
		
		ADD.L    (A0,D7.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADDQ.W   #5,D6
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #22,D6
		BLE.S    round1_inner
		CMPI.W   #64,D7
		BLT.S    round1
		
	round2:
		MOVEQ.L  #5,D6
		MOVEA.W  #4,A3
		MOVEA.L  A3,A4
		
	round2_inner:
		
		// G
		MOVE.L   D3,D4
		AND.L    D1,D4
		MOVE.L   D3,D5
		NOT.L    D5
		AND.L    D2,D5
		OR.L     D5,D4
		
		ADD.L    D4,D0
		
		MOVE.L   A4,D4
		ADD.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A0,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    A3,D6
		ADDQ.L   #1,A3
		ADDA.W   #16,A4
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #20,D6
		BLE.S    round2_inner
		CMPI.W   #128,D7
		BLT.S    round2
		
		MOVEQ.L  #7,D5
	round3:
		MOVEQ.L  #4,D6
		MOVEA.W  #20,A4
		
	round3_inner:
		
		// H
		MOVE.L   D1,D4
		EOR.L    D2,D4
		EOR.L    D3,D4
		
		ADD.L    D4,D0
		
		MOVE.L   A4,D4
		SUB.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A0,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    D5,D6
		EORI.L   #2,D5
		ADDA.W   #16,A4
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #23,D6
		BLE.S    round3_inner
		CMPI.W   #192,D7
		BLT.S    round3
		
		MOVEQ.L  #0,D5
	round4:
		MOVEQ.L  #6,D6
		MOVEA.W  #4,A3
		
	round4_inner:
		
		// I
		MOVE.L   D3,D4
		NOT.L    D4
		OR.L     D1,D4
		EOR.L    D2,D4
		
		ADD.L    D4,D0
		
		MOVE.L   D5,D4
		SUB.L    D7,D4
		ANDI.L   #0x3F,D4
		
		ADD.L    (A0,D4.W),D0
		ADD.L    (A2,D7.W),D0
		ROL.L    D6,D0
		ADD.L    D1,D0
		
		ADD.W    A3,D6
		ADDQ.L   #1,A3
		EORI.W   #32,D5
		
		ADDQ.W   #4,D7
		EXG      D3,D2
		EXG      D2,D1
		EXG      D1,D0
		
		CMPI.W   #21,D6
		BLE.S    round4_inner
		CMPI.W   #256,D7
		BLT.S    round4
		
		// Accumulate the result.
		ADD.L    D3,-(A1)
		ADD.L    D2,-(A1)
		ADD.L    D1,-(A1)
		ADD.L    D0,-(A1)
		
		MOVEM.L  (SP)+,D3-D7/A2-A4
		UNLK     A6
		RTS
	}
	
#else
	
	static void do_rounds( u32* const block, Buffer& buffer )
	{
		u32 a = buffer.a;
		u32 b = buffer.b;
		u32 c = buffer.c;
		u32 d = buffer.d;
		
		for ( int i = 0;  i < 16;  )
		{
			a = mix< 1 >( block, a,b,c,d, i,  7, i );  ++i;
			d = mix< 1 >( block, d,a,b,c, i, 12, i );  ++i;
			c = mix< 1 >( block, c,d,a,b, i, 17, i );  ++i;
			b = mix< 1 >( block, b,c,d,a, i, 22, i );  ++i;
		}
		
		for ( unsigned i = 16;  i < 32;  )
		{
			a = mix< 2 >( block, a,b,c,d, (i +  1) % 16,  5, i );  ++i;
			d = mix< 2 >( block, d,a,b,c, (i +  5) % 16,  9, i );  ++i;
			c = mix< 2 >( block, c,d,a,b, (i +  9) % 16, 14, i );  ++i;
			b = mix< 2 >( block, b,c,d,a, (i + 13) % 16, 20, i );  ++i;
		}
		
		for ( unsigned i = 32;  i < 48;  )
		{
			a = mix< 3 >( block, a,b,c,d, (48 - i +  5) % 16,  4, i );  ++i;
			d = mix< 3 >( block, d,a,b,c, (48 - i +  9) % 16, 11, i );  ++i;
			c = mix< 3 >( block, c,d,a,b, (48 - i + 13) % 16, 16, i );  ++i;
			b = mix< 3 >( block, b,c,d,a, (48 - i + 17) % 16, 23, i );  ++i;
		}
		
		for ( unsigned i = 48;  i < 64;  )
		{
			a = mix< 4 >( block, a,b,c,d, (64 - i + 0) % 16,  6, i );  ++i;
			d = mix< 4 >( block, d,a,b,c, (64 - i + 8) % 16, 10, i );  ++i;
			c = mix< 4 >( block, c,d,a,b, (64 - i + 0) % 16, 15, i );  ++i;
			b = mix< 4 >( block, b,c,d,a, (64 - i + 8) % 16, 21, i );  ++i;
		}
		
		buffer.a += a;
		buffer.b += b;
		buffer.c += c;
		buffer.d += d;
	}
	
#endif
	
	union Block
	{
		unsigned char  bytes[ 64 ];
		u32            words[ 16 ];
	};
	
	void Engine::DoBlock( const void* input )
	{
		u32 block[ 16 ];
		const u32* leBlock = reinterpret_cast< const u32* >( input );
		
		// Fill the block, swapping into big-endian.
		for ( int j = 0;  j < 16;  ++j )
		{
			block[ j ] = u32_from_little( leBlock[ j ] );
		}
		
		do_rounds( block, state );
		
		// Zero the block in case it contains sensitive material.
		std::fill( block,
		           block + 16,
		           0 );
		
		++blockCount;
	}
	
	void Engine::Finish( const void* input, unsigned bits )
	{
		const BitCount totalBits = blockCount * 512 + BitCount( bits );
		const unsigned inputBytes = bits / 8;
		const unsigned inputWords = inputBytes / 4;
		
		const unsigned bitsBeyondBoundary = (bits + 64) % 512;
		const unsigned bitsToPad = 512 - bitsBeyondBoundary;  // Between 1 and 512, inclusive
		const unsigned textBits = bits + bitsToPad + 64;
		
		const unsigned textBytes = textBits / 8;
		const unsigned textWords = textBytes / 4;
		const unsigned textBlocks = textWords / 16;
		
		Block block1, block2;
		std::vector< const Block* > blocks;
		
		const unsigned char* inputAsBytes = reinterpret_cast< const unsigned char* >( input );
		
		// Zero the padding blocks
		Block& endBlock = block2;
		Block& padBlock = (textBlocks > 1) ? block1
		                                   : block2;
		
		std::fill( block1.words, block1.words + 16, 0 );
		std::fill( block2.words, block2.words + 16, 0 );
		
		unsigned char& firstPadByte = padBlock.bytes[ inputBytes ];
		u32* bitLengthLoc = endBlock.words + 16 - 2;
		
		// Extra input bits and padding bits.
		
		// How many bits are left over that don't make a full byte?
		const unsigned extraBits = bits % 8;
		
		// Make a mask with ones where the extra bits are, and zeroes afterword.
		unsigned char extraMask = (0xFF << (8 - extraBits));
		
		const unsigned char& partialByte = inputAsBytes[ inputBytes ];
		
		// The first padding byte has leftover bits, if any, and a 1 appended.
		firstPadByte = (partialByte & extraMask) | (0x80 >> extraBits);
		
		// Append the bit length
		bitLengthLoc[ 0 ] = u32_from_little(  totalBits        & 0xFFFFFFFF );
		bitLengthLoc[ 1 ] = u32_from_little( (totalBits >> 32) & 0xFFFFFFFF );
		
		std::copy( inputAsBytes,
		           inputAsBytes + inputBytes,
		           padBlock.bytes );
		
		if ( &padBlock == &block1 )
		{
			DoBlock( &block1 );
		}
		
		DoBlock( &block2 );
		
		// Zero out pad blocks, since they may contain secrets copied from input
		std::fill( block1.words, block1.words + 16, 0 );
		std::fill( block2.words, block2.words + 16, 0 );
		
		state.a = little_u32( state.a );
		state.b = little_u32( state.b );
		state.c = little_u32( state.c );
		state.d = little_u32( state.d );
	}
	
	const Result& Engine::GetResult()
	{
		return reinterpret_cast< const Result& >( state );
	}
	
	Result Digest_Bits( const void* input, const BitCount& bitCount )
	{
		const Block* inputAsBlocks = reinterpret_cast< const Block* >( input );
		const unsigned blockCount = bitCount / 512;
		const Block* end = inputAsBlocks + blockCount;
		Engine engine;
		
		while ( inputAsBlocks < end )
		{
			engine.DoBlock( inputAsBlocks++ );
		}
		
		const unsigned bits = bitCount % 512;
		engine.Finish( inputAsBlocks, bits );
		
		return engine.GetResult();
	}
	
}  // namespace MD5
