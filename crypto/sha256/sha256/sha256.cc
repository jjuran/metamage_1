/*
	sha256.cc
	---------
*/

#include "sha256/sha256.hh"

// more-libc
#include "more/string.h"

// iota
#include "iota/endian.hh"

// sha256
#include "sha256/core.hh"


#pragma exceptions off


namespace crypto
{
	
	using iota::big_u32;
	
	
	void sha256_init( sha256_state& state )
	{
		state.n_blocks = 0;
		
		u32* h = state.digest.h;
		
		*h++ = 0x6a09e667;
		*h++ = 0xbb67ae85;
		*h++ = 0x3c6ef372;
		*h++ = 0xa54ff53a;
		*h++ = 0x510e527f;
		*h++ = 0x9b05688c;
		*h++ = 0x1f83d9ab;
		*h++ = 0x5be0cd19;
	}
	
	void sha256_digest_block( sha256_state& state, void const* data )
	{
		u32 block[ 64 ];
		
		if ( ! iota::is_little_endian() )
		{
			// Ensure the block is word-aligned.
			
			mempcpy( block, data, 64 );
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
		
		sha256_extend_block( block );
		
		sha256_rounds( state.digest, block );
		
		++state.n_blocks;
	}
	
	void sha256_finish( sha256_state&  state,
	                    void const*    data,
	                    size_t         n_bytes,
	                    int            n_more_bits )
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
		if ( ! iota::is_little_endian() )
		{
			final_data_u64[ 16 - 1 ] = total_bits;
		}
		else
		{
			u32* const length_ptr = final_data + 32 - 2;
			
			u32 const* const total_ptr = (u32*) &total_bits;
			
			length_ptr[ 0 ] = big_u32( total_ptr[ 1 ] );
			length_ptr[ 1 ] = big_u32( total_ptr[ 0 ] );
		}
		
		if ( using_both_blocks )
		{
			sha256_digest_block( state, final_data );
		}
		
		sha256_digest_block( state, end_block );
		
		u32* h = state.digest.h;
		
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
		*h = big_u32( *h );  h++;
	}
	
	sha256_hash sha256( const void* data, size_t n_bytes, int n_more_bits )
	{
		const unsigned n_last_bytes = n_bytes % 64;
		
		const char* p   = (const char*) data;
		const char* end = (const char*) data + n_bytes - n_last_bytes;
		
		sha256_state state;
		
		sha256_init( state );
		
		while ( p < end )
		{
			sha256_digest_block( state, p );
			
			p += 64;
		}
		
		sha256_finish( state, p, n_last_bytes, n_more_bits );
		
		return state.digest;
	}
	
}  // namespace crypto
