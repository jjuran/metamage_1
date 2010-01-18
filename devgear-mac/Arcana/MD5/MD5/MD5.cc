/*	======
 *	MD5.cc
 *	======
 */

// See RFC 1321, "The MD5 Message-Digest Algorithm".
// <http://www.faqs.org/rfcs/rfc1321.html>

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "MD5/MD5.hh"


namespace MD5
{
	
	typedef unsigned int Word;
	
	static inline unsigned int byteswap4( unsigned int word )
	{
		return (word &  0xFF)        << 24
			 | (word & (0xFF <<  8)) <<  8
			 | (word & (0xFF << 16)) >>  8
			 | (word & (0xFF << 24)) >> 24;
	}
	
	// MWC68K doesn't define __BIG_ENDIAN__, so we have to use __LITTLE_ENDIAN__.
	
	static inline unsigned int HostFromLittle32( unsigned int word )
	{
	#ifndef __LITTLE_ENDIAN__
		
		word = byteswap4( word );
		
	#endif
		
		return word;
	}
	
	static inline unsigned int LittleFromHost32( unsigned int word )
	{
	#ifndef __LITTLE_ENDIAN__
		
		word = byteswap4( word );
		
	#endif
		
		return word;
	}
	
	class Table
	{
		private:
			unsigned int data[ 64 ];
		
		public:
			Table();
			
			unsigned int operator[]( int i ) const  { return data[ i - 1 ]; }
	};
	
	static Table gTable;
	
	Table::Table()
	{
		using std::abs;
		using std::sin;
		
		for ( int i = 1;  i <= 64;  ++i )
		{
			data[ i - 1 ] = unsigned( 4294967296.0 * abs( sin( double( i ) ) ) );
		}
	}
	
	Buffer::Buffer() : a( byteswap4( 0x01234567 ) ),
	                   b( byteswap4( 0x89abcdef ) ),
	                   c( byteswap4( 0xfedcba98 ) ),
	                   d( byteswap4( 0x76543210 ) )
	{
	}
	
	static inline unsigned int rotate_left( unsigned int x, unsigned char bits )
	{
		return (x << bits) | (x >> (32 - bits));
	}
	
	static inline Word F( Word x, Word y, Word z )  { return (x & y)  |  (~x & z); }
	static inline Word G( Word x, Word y, Word z )  { return (x & z)  |  (y & ~z); }
	static inline Word H( Word x, Word y, Word z )  { return (x ^ y ^ z); }
	static inline Word I( Word x, Word y, Word z )  { return (y ^ (x | ~z)); }
	
	template < class Munger >
	class Operator
	{
		private:
			Munger munger;
			Word* block;
		
		public:
			Operator( Munger munger, Word* block ) : munger( munger ), block( block )  {}
			
			void operator()( Word&  a,
			                 Word   b,
			                 Word   c,
			                 Word   d,
			                 int    k,
			                 int    s,
			                 int    i ) const
			{
				a = b + rotate_left( a + munger( b, c, d ) + block[ k ] + gTable[ i ],
				                     s );
			}
	};
	
	template < class Munger >
	Operator< Munger > MakeOperator( Munger munger, Word* block )
	{
		return Operator< Munger >( munger, block );
	}
	
	template < class Operator >
	void Round1( Operator op, Buffer& buffer )
	{
		Word& a( buffer.a );
		Word& b( buffer.b );
		Word& c( buffer.c );
		Word& d( buffer.d );
		
		for ( int i = 1;  i <= 16;  )
		{
			op( a,b,c,d, i - 1,  7, i );  ++i;
			op( d,a,b,c, i - 1, 12, i );  ++i;
			op( c,d,a,b, i - 1, 17, i );  ++i;
			op( b,c,d,a, i - 1, 22, i );  ++i;
		}
	}
	
	template < class Operator >
	void Round2( Operator op, Buffer& buffer )
	{
		Word& a( buffer.a );
		Word& b( buffer.b );
		Word& c( buffer.c );
		Word& d( buffer.d );
		
		for ( int i = 16 + 1;  i <= 32;  )
		{
			op( a,b,c,d, (i     ) % 16,  5, i );  ++i;
			op( d,a,b,c, (i +  4) % 16,  9, i );  ++i;
			op( c,d,a,b, (i +  8) % 16, 14, i );  ++i;
			op( b,c,d,a, (i + 12) % 16, 20, i );  ++i;
		}
	}
	
	template < class Operator >
	void Round3( Operator op, Buffer& buffer )
	{
		Word& a( buffer.a );
		Word& b( buffer.b );
		Word& c( buffer.c );
		Word& d( buffer.d );
		
		for ( int i = 32 + 1;  i <= 48;  )
		{
			op( a,b,c,d, (48 - i +  6) % 16,  4, i );  ++i;
			op( d,a,b,c, (48 - i + 10) % 16, 11, i );  ++i;
			op( c,d,a,b, (48 - i + 14) % 16, 16, i );  ++i;
			op( b,c,d,a, (48 - i + 18) % 16, 23, i );  ++i;
		}
	}
	
	template < class Operator >
	void Round4( Operator op, Buffer& buffer )
	{
		Word& a( buffer.a );
		Word& b( buffer.b );
		Word& c( buffer.c );
		Word& d( buffer.d );
		
		for ( int i = 48 + 1;  i <= 64;  )
		{
			op( a,b,c,d, (64 - i + 1) % 16,  6, i );  ++i;
			op( d,a,b,c, (64 - i + 9) % 16, 10, i );  ++i;
			op( c,d,a,b, (64 - i + 1) % 16, 15, i );  ++i;
			op( b,c,d,a, (64 - i + 9) % 16, 21, i );  ++i;
		}
	}
	
	union Block
	{
		unsigned char  bytes[ 64 ];
		Word           words[ 16 ];
	};
	
	void Engine::DoBlock( const void* input )
	{
		Word block[ 16 ];
		const Word* leBlock = reinterpret_cast< const Word* >( input );
		
		// Fill the block, swapping into big-endian.
		for ( int j = 0;  j < 16;  ++j )
		{
			block[ j ] = HostFromLittle32( leBlock[ j ] );
		}
		
		Buffer oldState = state;
		
		Round1( MakeOperator( F, block ), state );
		Round2( MakeOperator( G, block ), state );
		Round3( MakeOperator( H, block ), state );
		Round4( MakeOperator( I, block ), state );
		
		// Zero the block in case it contains sensitive material.
		std::fill( block,
		           block + 16,
		           0 );
		
		state.a += oldState.a;
		state.b += oldState.b;
		state.c += oldState.c;
		state.d += oldState.d;
		
		++blockCount;
	}
	
	void Engine::Finish( const void* input, int bits )
	{
		BitCount totalBits = blockCount * 512 + BitCount( bits );
		int inputBytes = bits / 8;
		int inputWords = inputBytes / 4;
		
		int bitsBeyondBoundary = (bits + 64) % 512;
		int bitsToPad = 512 - bitsBeyondBoundary;  // Between 1 and 512, inclusive
		int textBits = bits + bitsToPad + 64;
		
		int textBytes = textBits / 8;
		int textWords = textBytes / 4;
		int textBlocks = textWords / 16;
		
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
		Word* bitLengthLoc = endBlock.words + 16 - 2;
		
		// Extra input bits and padding bits.
		
		// How many bits are left over that don't make a full byte?
		int extraBits = bits % 8;
		
		// Make a mask with ones where the extra bits are, and zeroes afterword.
		unsigned char extraMask = (0xFF << (8 - extraBits));
		
		const unsigned char& partialByte = inputAsBytes[ inputBytes ];
		
		// The first padding byte has leftover bits, if any, and a 1 appended.
		firstPadByte = (partialByte & extraMask) | (0x80 >> extraBits);
		
		// Append the bit length
		bitLengthLoc[ 0 ] = HostFromLittle32(  totalBits        & 0xFFFFFFFF );
		bitLengthLoc[ 1 ] = HostFromLittle32( (totalBits >> 32) & 0xFFFFFFFF );
		
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
		
		state.a = LittleFromHost32( state.a );
		state.b = LittleFromHost32( state.b );
		state.c = LittleFromHost32( state.c );
		state.d = LittleFromHost32( state.d );
	}
	
	const Result& Engine::GetResult()
	{
		return reinterpret_cast< const Result& >( state );
	}
	
	Result Digest_Bits( const void* input, const BitCount& bitCount )
	{
		const Block* inputAsBlocks = reinterpret_cast< const Block* >( input );
		int blockCount = bitCount / 512;
		const Block* end = inputAsBlocks + blockCount;
		Engine engine;
		
		while ( inputAsBlocks < end )
		{
			engine.DoBlock( inputAsBlocks++ );
		}
		
		int bits = bitCount % 512;
		engine.Finish( inputAsBlocks, bits );
		
		return engine.GetResult();
	}
	
}  // namespace MD5

