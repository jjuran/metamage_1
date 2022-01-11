/*
	utf8.cc
	-------
*/

#include "encoding/utf8.hh"


namespace chars
{
	
	static inline bool is_continuation_byte( utf8_t c )
	{
		return (c & 0xC0) == 0x80;
	}
	
	void put_code_point_into_utf8( unichar_t uc, unsigned n_bytes, char* p )
	{
		switch ( n_bytes )
		{
			default:
			case 0:
				break;
			
			case 1:
				*p++ = uc;
				break;
			
			case 2:
				*p++ = 0xC0 | (uc >> 6 & 0x1F);
				*p++ = 0x80 | (uc      & 0x3F);
				break;
			
			case 3:
				*p++ = 0xE0 | (uc >> 12 & 0x0F);
				*p++ = 0x80 | (uc >>  6 & 0x3F);
				*p++ = 0x80 | (uc       & 0x3F);
				break;
			
			case 4:
				*p++ = 0xF0 | (uc >> 18 & 0x07);
				*p++ = 0x80 | (uc >> 12 & 0x3F);
				*p++ = 0x80 | (uc >>  6 & 0x3F);
				*p++ = 0x80 | (uc       & 0x3F);
				break;
		}
	}
	
	unichar_t get_next_code_point_from_utf8( const char*& p, const char* end )
	{
		const unsigned n_bytes = count_utf8_bytes_in_char( p[0] );
		
		if ( n_bytes == 0  ||  p + n_bytes > end )
		{
			return unichar_t( -1 );
		}
		
		switch ( n_bytes )
		{
			// These return or fall through
			case 4:  if ( !is_continuation_byte( p[3] ) )  return unichar_t( -1 );
			case 3:  if ( !is_continuation_byte( p[2] ) )  return unichar_t( -1 );
			case 2:  if ( !is_continuation_byte( p[1] ) )  return unichar_t( -1 );
			
			default:
				break;
		}
		
		unichar_t result = unichar_t( -1 );
		
		switch ( n_bytes )
		{
			case 1:
				result = p[0];
				break;
			
			case 2:
				result = (p[0] & 0x1F) << 6
				       | (p[1] & 0x3F);
				break;
			
			case 3:
				result = (p[0] & 0x0F) << 12
				       | (p[1] & 0x3F) <<  6
				       | (p[2] & 0x3F);
				
				if ( result < 0x800  ||  (result >= 0xD800  &&  result < 0xE000) )
				{
					// overlong encoding, or surrogate
					return unichar_t( -1 );
				}
				
				break;
			
			case 4:
				result = (p[0] & 0x07) << 18
				       | (p[1] & 0x3F) << 12
				       | (p[2] & 0x3F) <<  6
				       | (p[3] & 0x3F);
				
				if ( result < 0x10000 )
				{
					return unichar_t( -1 );
				}
				
				break;
			
			default:
				return unichar_t( -1 );
		}
		
		p += n_bytes;
		
		return result;
	}
	
}
