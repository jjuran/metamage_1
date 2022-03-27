/*
	core.cc
	-------
*/

#include "sha1/core.hh"


namespace crypto
{
	
	static inline u32 rotate_left( u32 x, unsigned char bits )
	{
		return (x << bits) | (x >> (32 - bits));
	}
	
	
	template < int round >
	struct munger;
	
	template <>
	struct munger< 1 >
	{
		static const u32 k = 0x5A827999;
		
		static u32 f( u32 x, u32 y, u32 z )  { return (x & y)  |  (~x & z); }
	};
	
	template <>
	struct munger< 2 >
	{
		static const u32 k = 0x6ED9EBA1;
		
		static u32 f( u32 x, u32 y, u32 z )  { return x ^ y ^ z; }
	};
	
	template <>
	struct munger< 3 >
	{
		static const u32 k = 0x8F1BBCDC;
		
		static u32 f( u32 x, u32 y, u32 z )  { return (x&y) | (x&z) | (y&z); }
	};
	
	template <>
	struct munger< 4 >
	{
		static const u32 k = 0xCA62C1D6;
		
		static u32 f( u32 x, u32 y, u32 z )  { return x ^ y ^ z; }
	};
	
	
	template < int round >
	static u32 mix( u32 const*  w,
	                u32         a,
	                u32         b,
	                u32         c,
	                u32         d,
	                int         i )
	{
		typedef munger< round > munge;
		
		a = rotate_left( a, 5 );
		
		a += munge::f( b, c, d ) + munge::k;
		
		a += w[ i ];
		
		return a;
	}
	
#ifdef __MC68K__
	
	// Defined in core.68k.cc
	
#else
	
	void sha1_extend_block( u32* block )
	{
		u32* w = block;
		
		for ( int i = 16;  i < 80;  ++i )
		{
			const u32 x = w[ i - 3 ] ^ w[ i - 8 ] ^ w[ i - 14 ] ^ w[ i - 16 ];
			
			w[ i ] = rotate_left( x, 1 );
		}
	}
	
	void sha1_rounds( sha1_digest& digest, u32* const block )
	{
		u32* h = digest.h;
		
		u32 a = *h++;
		u32 b = *h++;
		u32 c = *h++;
		u32 d = *h++;
		u32 e = *h++;
		
		for ( int i = 0;  i < 20;  )
		{
			e += mix< 1 >( block, a,b,c,d, i++ );  b = rotate_left( b, 30 );
			d += mix< 1 >( block, e,a,b,c, i++ );  a = rotate_left( a, 30 );
			c += mix< 1 >( block, d,e,a,b, i++ );  e = rotate_left( e, 30 );
			b += mix< 1 >( block, c,d,e,a, i++ );  d = rotate_left( d, 30 );
			a += mix< 1 >( block, b,c,d,e, i++ );  c = rotate_left( c, 30 );
		}
		
		for ( unsigned i = 20;  i < 40;  )
		{
			e += mix< 2 >( block, a,b,c,d, i++ );  b = rotate_left( b, 30 );
			d += mix< 2 >( block, e,a,b,c, i++ );  a = rotate_left( a, 30 );
			c += mix< 2 >( block, d,e,a,b, i++ );  e = rotate_left( e, 30 );
			b += mix< 2 >( block, c,d,e,a, i++ );  d = rotate_left( d, 30 );
			a += mix< 2 >( block, b,c,d,e, i++ );  c = rotate_left( c, 30 );
		}
		
		for ( unsigned i = 40;  i < 60;  )
		{
			e += mix< 3 >( block, a,b,c,d, i++ );  b = rotate_left( b, 30 );
			d += mix< 3 >( block, e,a,b,c, i++ );  a = rotate_left( a, 30 );
			c += mix< 3 >( block, d,e,a,b, i++ );  e = rotate_left( e, 30 );
			b += mix< 3 >( block, c,d,e,a, i++ );  d = rotate_left( d, 30 );
			a += mix< 3 >( block, b,c,d,e, i++ );  c = rotate_left( c, 30 );
		}
		
		for ( unsigned i = 60;  i < 80;  )
		{
			e += mix< 4 >( block, a,b,c,d, i++ );  b = rotate_left( b, 30 );
			d += mix< 4 >( block, e,a,b,c, i++ );  a = rotate_left( a, 30 );
			c += mix< 4 >( block, d,e,a,b, i++ );  e = rotate_left( e, 30 );
			b += mix< 4 >( block, c,d,e,a, i++ );  d = rotate_left( d, 30 );
			a += mix< 4 >( block, b,c,d,e, i++ );  c = rotate_left( c, 30 );
		}
		
		*--h += e;
		*--h += d;
		*--h += c;
		*--h += b;
		*--h += a;
	}
	
#endif
	
}  // namespace crypto
