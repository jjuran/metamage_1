/*
	adler32.cc
	----------
*/

#include "adler32/adler32.hh"


namespace adler32
{
	
	enum
	{
		modulus = 65521,
		
		max_dividend = modulus * 65535,
	};
	
	hash_t checksum( const byte_t* data, size_t n_bytes )
	{
		const byte_t* end = data + n_bytes;
		
		hash_t series     = 1;
		hash_t metaseries = 0;
		
		while ( data < end )
		{
			series += *data++;
			
			if ( (long) series < 0 )
			{
				series %= modulus;
			}
			
			metaseries += series;
			
			if ( (long) metaseries < 0 )
			{
				metaseries %= modulus;
			}
		}
		
		series     %= modulus;
		metaseries %= modulus;
		
		return metaseries << 16 | series;
	}
	
}
