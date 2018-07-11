/*
	byte_range.hh
	-------------
*/

#ifndef IOTA_BYTERANGE_HH
#define IOTA_BYTERANGE_HH


namespace iota
{
	
	enum byte_range
	{
		Byterange_empty = 0x00000000,
		
		Byterange_all   = 0x00000100,
		
		Byterange_max   = 0x00ff0100,
	};
	
	inline
	byte_range make_byte_range()
	{
		return Byterange_empty;
	}
	
	inline
	byte_range make_byte_range_prechecked( unsigned char a, unsigned char z )
	{
		return byte_range( a << 16 | z + 1 );
	}
	
	inline
	byte_range make_byte_range( unsigned char a, unsigned char z )
	{
		return a <= z ? make_byte_range_prechecked( a, z ) : Byterange_empty;
	}
	
	inline
	byte_range make_byte_range( unsigned char c )
	{
		return make_byte_range( c, c );
	}
	
	inline
	bool empty( byte_range br )
	{
		return br == Byterange_empty;
	}
	
	inline
	unsigned short begin( byte_range br )
	{
		return br >> 16;
	}
	
	inline
	unsigned short end( byte_range br )
	{
		return br;
	}
	
	inline
	unsigned char front( byte_range br )
	{
		return begin( br );
	}
	
	inline
	unsigned char back( byte_range br )
	{
		return end( br ) - 1;
	}
	
	inline
	unsigned short size( byte_range br )
	{
		return end( br ) - begin( br );
	}
	
	inline
	bool valid( byte_range br )
	{
		if ( empty( br ) )
		{
			return true;
		}
		
		return end( br ) <= 0x100  &&  begin( br ) < end( br );
	}
	
	inline
	bool contains( byte_range br, unsigned char c )
	{
		return c >= begin( br )  &&  c < end( br );
	}
	
	inline
	byte_range intersection( byte_range a, byte_range b )
	{
		const unsigned short a_begin = begin( a );
		const unsigned short a_end   = end  ( a );
		
		const unsigned short b_begin = begin( b );
		const unsigned short b_end   = end  ( b );
		
		const unsigned short begin = a_begin > b_begin ? a_begin : b_begin;
		const unsigned short end   = a_end   < b_end   ? a_end   : b_end;
		
		return begin < end ? make_byte_range( begin, end - 1 )
		                   : make_byte_range();
	}
	
}

#endif
