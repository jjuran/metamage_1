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
		
		max_i32_dividend = 0x7fffffff / modulus * modulus,
	};
	
#ifdef __MC68K__
	
	static inline
	asm
	hash_t modulo( hash_t x : __D0 )
	{
		MOVE.W   #modulus,D1
		DIVU.W   D1,D0
		SWAP     D0
		EXT.L    D0
	}
	
#else
	
	static inline
	hash_t modulo( hash_t x )
	{
		return x % modulus;
	}
	
#endif
	
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
				series -= max_i32_dividend;
			}
			
			metaseries += series;
			
			if ( (signed_t) metaseries < 0 )
			{
				metaseries -= max_i32_dividend;
			}
		}
		
		series     = modulo( series     );
		metaseries = modulo( metaseries );
		
		return metaseries << 16 | series;
	}
	
}
