/*
	is_binary_data.cc
	-----------------
*/

#include "Genie/Utilities/is_binary_data.hh"


namespace Genie
{
	
	typedef unsigned long size_t;
	
	static inline size_t min( size_t a, size_t b )
	{
		return a < b ? a : b;
	}
	
	bool is_binary_data( const char* data, size_t n_bytes )
	{
		// bitmap of which control characters make a file 'binary' (i.e. non-text)
		
		const unsigned mask = 1 << '\x00'
		                    | 1 << '\x01'
		                    | 1 << '\x02'
		                    | 1 << '\x03'
		                    | 1 << '\x04'
		                    | 1 << '\x05'
		                    | 1 << '\x06'
		                    | 1 << '\x07'
		                    | 1 << '\x08'
		                    | 0 << '\t'  // Tab
		                    | 0 << '\n'  // Line Feed
		                    | 1 << '\x0b'
		                    | 1 << '\x0c'
		                    | 0 << '\r'  // Return
		                    | 1 << '\x0e'
		                    | 1 << '\x0f'
		                    | 1 << '\x10'
		                    | 1 << '\x11'
		                    | 1 << '\x12'
		                    | 1 << '\x13'
		                    | 1 << '\x14'
		                    | 1 << '\x15'
		                    | 1 << '\x16'
		                    | 1 << '\x17'
		                    | 1 << '\x18'
		                    | 1 << '\x19'
		                    | 1 << '\x1a'
		                    | 1 << '\x1b'
		                    | 1 << '\x1c'
		                    | 1 << '\x1d'
		                    | 1 << '\x1e'
		                    | 1 << '\x1f';
		
		// Enough of the file header to tell if it's 'binary'.
		// PEF is text through the first 12, so make it 16.
		const size_t enough_bytes = 16;
		
		const char* end = data + min( n_bytes, enough_bytes );
		
		for ( const char* p = data;  p < end;  ++p )
		{
			const char c = *p;
			
			if ( c < 32  &&  1 << c & mask )
			{
				// Control character found.
				
				return true;
			}
		}
		
		return false;
	}
	
}

