/*
	quote.hh
	--------
*/

#ifndef VXO_LIB_QUOTE_HH
#define VXO_LIB_QUOTE_HH


namespace vxo
{
	
	inline
	unsigned worst_case_quoted_length( unsigned input_length )
	{
		/*
			At worst, an input byte requires a 4-byte escape sequence.
			Even if the input is MacRoman converted to UTF-8, a non-ASCII
			MacRoman character converts to a 2- or 3-byte UTF-8 sequence of
			non-ASCII bytes, which are emitted as is rather than escaped, so
			four bytes per input byte remains the worst case.
			
			Finally, the enclosing quotes add another two bytes.
		*/
		
		return input_length * 4 + 2;
	}
	
	unsigned quoted_length( unsigned char c );
	
	unsigned quoted_length( const char* p, unsigned n );
	
	char* copy_quotable_byte( char* q, unsigned char c );
	
	char* quote_byte( char* q, unsigned char c );
	
	char* quote_string( const char* p, unsigned n, char* q );
	
}

#endif
