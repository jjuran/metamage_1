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
	
	/*
		Unfortunately, Metrowerks C++ 2.4.1 tries to use the template overloads
		of begin() and end() instead of the byte_range overloads, even though
		byte_range is an exact match.  So we have to call them something else.
	*/
	
	inline
	unsigned short _begin( byte_range br )
	{
		return br >> 16;
	}
	
	inline
	unsigned short _end( byte_range br )
	{
		return br;
	}
	
	inline
	unsigned short begin( byte_range br )
	{
		return _begin( br );
	}
	
	inline
	unsigned short end( byte_range br )
	{
		return _end( br );
	}
	
	inline
	unsigned char front( byte_range br )
	{
		return _begin( br );
	}
	
	inline
	unsigned char back( byte_range br )
	{
		return _end( br ) - 1;
	}
	
	inline
	unsigned short size( byte_range br )
	{
		return _end( br ) - _begin( br );
	}
	
	inline
	bool valid( byte_range br )
	{
		if ( empty( br ) )
		{
			return true;
		}
		
		return _end( br ) <= 0x100  &&  _begin( br ) < _end( br );
	}
	
	inline
	bool contains( byte_range br, unsigned char c )
	{
		return c >= _begin( br )  &&  c < _end( br );
	}
	
	inline
	byte_range intersection( byte_range a, byte_range b )
	{
		const unsigned short a_begin = _begin( a );
		const unsigned short a_end   = _end  ( a );
		
		const unsigned short b_begin = _begin( b );
		const unsigned short b_end   = _end  ( b );
		
		const unsigned short begin = a_begin > b_begin ? a_begin : b_begin;
		const unsigned short end   = a_end   < b_end   ? a_end   : b_end;
		
		return begin < end ? make_byte_range( begin, end - 1 )
		                   : make_byte_range();
	}
	
}

#endif
