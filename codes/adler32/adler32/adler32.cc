/*
	adler32.cc
	----------
*/

#include "adler32/adler32.hh"


#if __LP64__
	
	typedef int  signed_t;
	
#else
	
	typedef long signed_t;
	
#endif


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
			
			if ( (signed_t) series < 0 )
			{
				series %= modulus;
			}
			
			metaseries += series;
			
			if ( (signed_t) metaseries < 0 )
			{
				metaseries %= modulus;
			}
		}
		
		series     %= modulus;
		metaseries %= modulus;
		
		return metaseries << 16 | series;
	}
	
}
