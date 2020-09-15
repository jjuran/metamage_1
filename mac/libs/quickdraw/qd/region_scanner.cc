/*
	region_scanner.cc
	-----------------
*/

#include "qd/region_scanner.hh"

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// quickdraw
#include "qd/region_detail.hh"


namespace quickdraw
{
	
	typedef unsigned short uint16_t;
	
	
	static bool bit_eq( const uint16_t* a, const uint16_t* b, unsigned n )
	{
		const int bits_per_word = 16;
		
		while ( n >= bits_per_word )
		{
			if ( *a++ != *b++ )
			{
				return false;
			}
			
			n -= bits_per_word;
		}
		
		if ( n > 0 )
		{
			const uint16_t mask = iota::big_u16( ~((1 << (16 - n)) - 1) );
			
			const uint16_t _a = *a & mask;
			const uint16_t _b = *b & mask;
			
			if ( _a != _b )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static void bitxor( uint16_t*        dst,
	                    const uint16_t*  a,
	                    const uint16_t*  b,
	                    unsigned         n )
	{
		const int bits_per_word = 16;
		
		while ( n >= bits_per_word )
		{
			n -= bits_per_word;
			
			*dst++ = *a++ ^ *b++;
		}
		
		if ( n > 0 )
		{
			const uint16_t mask = iota::big_u16( ~((1 << (16 - n)) - 1) );
			
			*dst = (*a ^ *b) & mask;
		}
	}
	
	static bool mask_to_region_line( short left, const uint16_t* mask, unsigned size, short*& r )
	{
		uint16_t negated = 0;
		
		size /= 2;
		
		for ( int i = 0;  i < size;  ++i )
		{
			uint16_t word = mask[ i ];
			
			word ^= negated;
			
			if ( word != 0 )
			{
				word = iota::u16_from_big( word );
				
				for ( int j = 15;  j >= 0;  --j )
				{
					if ( word & (1 << j) )
					{
						*r++ = left + i * 16 + 15 - j;
						
						negated = ~negated;
						word    = ~word;
					}
				}
			}
		}
		
		return negated;
	}
	
	static void scan_line( short            h,
	                       short            v,
	                       const uint16_t*  bits,
	                       unsigned         size,
	                       short*&          r )
	{
		*r++ = v;
		
		const bool odd = mask_to_region_line( h, bits, size, r );
		
		if ( odd )
		{
			*r++ = h + size * 8;
		}
		
		*r++ = Region_end;
	}
	
	
	region_scanner::region_scanner( short* out, uint16_t* temp, unsigned size )
	:
		its_mark( out ),
		its_temp( temp ),
		its_line_size( size )
	{
		memset( temp, '\0', size );
	}
	
	bool region_scanner::scan( short            h,
	                           short            v,
	                           const uint16_t*  bits,
	                           const uint16_t*  prev,
	                           int              margin )
	{
		const int n_bits = its_line_size * 8 - margin;
		
		if ( !bit_eq( bits, prev, n_bits ) )
		{
			bitxor( its_temp, prev, bits, n_bits );
			
			scan_line( h, v, its_temp, its_line_size, its_mark );
			
			return true;
		}
		
		return false;
	}
	
	void region_scanner::finish( short            h,
	                             short            v,
	                             const uint16_t*  prev,
	                             int              margin )
	{
		memset( its_temp, '\0', its_line_size );
		
		scan( h, v, its_temp, prev, margin );
		
		*its_mark++ = Region_end;
	}
	
}
