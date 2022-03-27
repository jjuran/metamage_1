/*
	core.cc
	-------
*/

#include "sha256/core.hh"

// sha256
#include "sha256/table.hh"


namespace crypto
{
	
	static inline u32 rotate_right( u32 x, unsigned char bits )
	{
		return (x >> bits) | (x << (32 - bits));
	}
	
	
#ifdef __MC68K__
	
	// Defined in core.68k.cc
	
#else
	
	void sha256_extend_block( u32* block )
	{
		u32* w = block;
		
		for ( int i = 16;  i < 64;  ++i )
		{
			const u32 s0 = rotate_right( w[ i - 15 ],  7 )
			             ^ rotate_right( w[ i - 15 ], 18 )
			             ^ w[ i - 15 ] >> 3;
			
			const u32 s1 = rotate_right( w[ i - 2 ], 17 )
			             ^ rotate_right( w[ i - 2 ], 19 )
			             ^ w[ i - 2 ] >> 10;
			
			w[ i ] = w[ i - 16 ] + s0 + w[ i - 7 ] + s1;
		}
	}
	
	void sha256_rounds( sha256_hash& hash, u32* const block )
	{
		u32 const* const k = sha256_table;
		u32 const* const w = block;
		
		u32* hh = hash.h;
		
		u32 a = *hh++;
		u32 b = *hh++;
		u32 c = *hh++;
		u32 d = *hh++;
		u32 e = *hh++;
		u32 f = *hh++;
		u32 g = *hh++;
		u32 h = *hh++;
		
		for ( int i = 0;  i < 64;  ++i )
		{
			const u32 s1 = rotate_right( e,  6 )
			             ^ rotate_right( e, 11 )
			             ^ rotate_right( e, 25 );
			
			const u32 ch = (e & f) ^ (~e & g);
			
			const u32 temp1 = h + s1 + ch + k[ i ] + w[ i ];
			
			const u32 s0 = rotate_right( a,  2 )
			             ^ rotate_right( a, 13 )
			             ^ rotate_right( a, 22 );
			
			const u32 maj = (a & b) ^ (a & c) ^ (b & c);
			
			const u32 temp2 = s0 + maj;
			
			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}
		
		*--hh += h;
		*--hh += g;
		*--hh += f;
		*--hh += e;
		*--hh += d;
		*--hh += c;
		*--hh += b;
		*--hh += a;
	}
	
	#endif
	
}  // namespace crypto
