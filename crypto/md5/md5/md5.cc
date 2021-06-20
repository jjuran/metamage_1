/*
	md5.cc
	------
*/

/*
	See RFC 1321, "The MD5 Message-Digest Algorithm".
	<http://www.faqs.org/rfcs/rfc1321.html>
*/

#include "md5/md5.hh"

// more-libc
#include "more/string.h"

// iota
#include "iota/endian.hh"

// md5
#include "md5/rounds.hh"


#pragma exceptions off


namespace crypto
{
	
	using iota::swap_4_bytes;
	using iota::little_u32;
	
	
	void md5_init( md5_state& state )
	{
		state.n_blocks = 0;
		
		state.digest.a = swap_4_bytes( 0x01234567 );
		state.digest.b = swap_4_bytes( 0x89abcdef );
		state.digest.c = swap_4_bytes( 0xfedcba98 );
		state.digest.d = swap_4_bytes( 0x76543210 );
	}
	
	void md5_digest_block( md5_state& state, void const* data )
	{
		u32 block[ 16 ];
		
		if ( iota::is_little_endian() )
		{
			// Ensure the block is word-aligned.
			
			mempcpy( block, data, sizeof block );
		}
		else
		{
			// Byte-swap each word.
			
			const char* p = (const char*) data;
			
			char* q = (char*) block;
			
			for ( int i = 0;  i < 16; ++i )
			{
				q += 4;
				
				char* r = q;
				
				*--r = *p++;
				*--r = *p++;
				*--r = *p++;
				*--r = *p++;
			}
		}
		
		md5_rounds( state.digest, block );
		
		++state.n_blocks;
	}
	
	void md5_finish( md5_state&   state,
	                 void const*  data,
	                 size_t       n_bytes,
	                 int          n_more_bits )
	{
		typedef unsigned char u8;
		
		const unsigned n_bits = n_bytes * 8 + n_more_bits;
		
		const u64 total_bits = state.n_blocks * 512 + n_bits;
		
		const unsigned overflow_bits = (n_bits + 64) % 512;
		const unsigned bits_to_pad = 512 - overflow_bits;  // between 1 and 512
		const unsigned n_final_bits = n_bits + bits_to_pad + 64;
		
		const unsigned n_final_blocks = n_final_bits / 512;
		
		const bool using_both_blocks = n_final_blocks - 1;
		
		u64  final_data_u64[ 16 ] = { 0 };  // ensure 64-bit alignment
		u32* final_data = (u32*) final_data_u64;
		
		u32* const end_block = final_data + 16;
		u32* const pad_block = using_both_blocks ? final_data : end_block;
		
		mempcpy( pad_block, data, n_bytes );
		
		u8* const pad_byte_ptr = (u8*) pad_block + n_bytes;
		
		// Extra input bits and padding bits.
		
		// Make a mask with ones where the extra bits are, and zeroes afterword.
		unsigned char bits_mask = (0xFF << (8 - n_more_bits));
		
		// The first padding byte has leftover bits, if any, and a 1 appended.
		
		*pad_byte_ptr &= bits_mask;
		*pad_byte_ptr |= (0x80 >> n_more_bits);
		
		// Append the bit length
		if ( iota::is_little_endian() )
		{
			final_data_u64[ 16 - 1 ] = total_bits;
		}
		else
		{
			u32* const length_ptr = final_data + 32 - 2;
			
			u32 const* const total_ptr = (u32*) &total_bits;
			
			length_ptr[ 0 ] = little_u32( total_ptr[ 1 ] );
			length_ptr[ 1 ] = little_u32( total_ptr[ 0 ] );
		}
		
		if ( using_both_blocks )
		{
			md5_digest_block( state, final_data );
		}
		
		md5_digest_block( state, end_block );
		
		md5_digest& digest = state.digest;
		
		digest.a = little_u32( digest.a );
		digest.b = little_u32( digest.b );
		digest.c = little_u32( digest.c );
		digest.d = little_u32( digest.d );
	}
	
	md5_digest md5( const void* data, size_t n_bytes, int n_more_bits )
	{
		const unsigned n_last_bytes = n_bytes % 64;
		
		const char* p   = (const char*) data;
		const char* end = (const char*) data + n_bytes - n_last_bytes;
		
		md5_engine engine;
		
		while ( p < end )
		{
			engine.digest_block( p );
			
			p += 64;
		}
		
		return engine.finish( p, n_last_bytes, n_more_bits );
	}
	
}  // namespace crypto
