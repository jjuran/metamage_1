/*
	core.cc
	-------
*/

/*
	See RFC 1321, "The MD5 Message-Digest Algorithm".
	<http://www.faqs.org/rfcs/rfc1321.html>
*/

#include "md5/core.hh"

// iota
#include "iota/endian.hh"

// md5
#include "md5/table.hh"


namespace crypto
{
	
	static inline u32 rotate_left( u32 x, unsigned char bits )
	{
		return (x << bits) | (x >> (32 - bits));
	}
	
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
		w += munger< round >::f( x, y, z ) + block[ k ] + md5_table[ i ];
		
		return x + rotate_left( w, s );
	}
	
#ifdef __MC68K__
	
	// defined in core.68k.cc
	
#else
	
	void md5_rounds( md5_digest& digest, u32* const block )
	{
		u32 a = digest.a;
		u32 b = digest.b;
		u32 c = digest.c;
		u32 d = digest.d;
		
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
		
		digest.a += a;
		digest.b += b;
		digest.c += c;
		digest.d += d;
	}
	
#endif
	
}  // namespace crypto
